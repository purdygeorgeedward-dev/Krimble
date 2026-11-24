/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004-2008 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2004 Clarence Dang <dang@kde.org>
 *  SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
 *  SPDX-FileCopyrightText: 2004 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_brushop.h"

#include <QRect>

#include <kis_image.h>
#include <kis_vec.h>
#include <kis_debug.h>

#include <KoColorTransformation.h>
#include <KoColor.h>

#include <kis_brush.h>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_brush_based_paintop_settings.h>
#include <kis_lod_transform.h>
#include <kis_paintop_plugin_utils.h>
#include "krita_utils.h"
#include "kis_algebra_2d.h"
#include <KisDabRenderingExecutor.h>
#include <KisDabCacheUtils.h>
#include <KisRenderedDab.h>
#include <kis_tool_freehand.h>
#include "KisBrushOpResources.h"

#include <KisRunnableStrokeJobData.h>
#include <KisRunnableStrokeJobUtils.h>
#include <KisRunnableStrokeJobsInterface.h>

#include <QThread>
#include "kis_image_config.h"
#include "kis_wrapped_rect.h"

KisBrushOp::KisBrushOp(const KisPaintOpSettingsSP settings, KisPainter *painter, KisNodeSP node, KisImageSP image)
    : KisBrushBasedPaintOp(settings, painter, SupportsGradientMode | SupportsLightnessMode)
    , m_sizeOption(settings.data())
    , m_ratioOption(settings.data())
    , m_rateOption(settings.data())
    , m_softnessOption(settings.data())
    , m_lightnessStrengthOption(settings.data())
    , m_spacingOption(settings.data())
    , m_scatterOption(settings.data())
    , m_sharpnessOption(settings.data())
    , m_rotationOption(settings.data())
    , m_opacityOption(settings.data(), node)
    , m_avgSpacing(50)
    , m_avgNumDabs(50)
    , m_avgUpdateTimePerDab(50)
    , m_idealNumRects(KisImageConfig(true).maxNumberOfThreads())
    , m_minUpdatePeriod(10)
    , m_maxUpdatePeriod(100)
{
    Q_UNUSED(image);
    Q_ASSERT(settings);

    m_airbrushData.read(settings.data());
    m_rotationOption.applyFanCornersInfo(this);

    m_precisionOption.setHasImprecisePositionOptions(
        m_precisionOption.hasImprecisePositionOptions()
        || m_scatterOption.isChecked() || m_rotationOption.isChecked()
        || m_airbrushData.isChecked);

    m_brush->notifyBrushIsGoingToBeClonedForStroke();

    KisBrushSP baseBrush = m_brush;
    auto resourcesFactory =
        [baseBrush, settings, painter] () {
            KisDabCacheUtils::DabRenderingResources *resources =
                new KisBrushOpResources(settings, painter);
            resources->brush = baseBrush->clone().dynamicCast<KisBrush>();

            return resources;
        };

    m_dabExecutor.reset(
        new KisDabRenderingExecutor(
                    painter->device()->compositionSourceColorSpace(),
                    resourcesFactory,
                    painter->runnableStrokeJobsInterface(),
                    &m_mirrorOption,
                    &m_precisionOption));
}

KisBrushOp::~KisBrushOp()
{
}

KisSpacingInformation KisBrushOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()->device()) return KisSpacingInformation(1.0);

    KisBrushSP brush = m_brush;
    Q_ASSERT(brush);
    if (!brush)
        return KisSpacingInformation(1.0);

    if (!brush->canPaintFor(info))
        return KisSpacingInformation(1.0);

    qreal scale = m_sizeOption.apply(info);
    scale *= KisLodTransform::lodToScale(painter()->device());
    if (checkSizeTooSmall(scale)) return KisSpacingInformation();

    qreal rotation = m_rotationOption.apply(info);
    qreal ratio = m_ratioOption.apply(info);

    KisDabShape shape(scale, ratio, rotation);
    QPointF cursorPos =
        m_scatterOption.apply(info,
                              brush->maskWidth(shape, 0, 0, info),
                              brush->maskHeight(shape, 0, 0, info));

    qreal dabOpacity = OPACITY_OPAQUE_F;
    qreal dabFlow = OPACITY_OPAQUE_F;

    m_opacityOption.apply(info, &dabOpacity, &dabFlow);

    KisDabCacheUtils::DabRequestInfo request(painter()->paintColor(),
                                             cursorPos,
                                             shape,
                                             info,
                                             m_softnessOption.apply(info),
                                             m_lightnessStrengthOption.apply(info));

    m_dabExecutor->addDab(request, dabOpacity, dabFlow);

    KisSpacingInformation spacingInfo =
        effectiveSpacing(scale, rotation, &m_airbrushData, &m_spacingOption, info);

    m_avgSpacing(spacingInfo.scalarApprox());

    return spacingInfo;
}

struct KisBrushOp::UpdateSharedState
{
    KisPainter *painter = 0;
    QList<KisRenderedDab> dabsQueue;

    QVector<QPointF> dabPoints;
    QElapsedTimer dabRenderingTimer;

    QVector<QRect> allDirtyRects;
};

void KisBrushOp::addMirroringJobs(Qt::Orientation direction,
                                  QVector<QRect> &rects,
                                  UpdateSharedStateSP state,
                                  QVector<KisRunnableStrokeJobData*> &jobs)
{
    KritaUtils::addJobSequential(jobs, nullptr);

    KisFixedPaintDeviceSP prevDabDevice = 0;
    for (KisRenderedDab &dab : state->dabsQueue) {
        const bool skipMirrorPixels = prevDabDevice && prevDabDevice == dab.device;

        KritaUtils::addJobConcurrent(jobs,
            [state, &dab, direction, skipMirrorPixels] () {
                state->painter->mirrorDab(direction, &dab, skipMirrorPixels);
            }
        );

        prevDabDevice = dab.device;
    }

    KritaUtils::addJobSequential(jobs, nullptr);

    for (QRect &rc : rects) {
        state->painter->mirrorRect(direction, &rc);

        KritaUtils::addJobConcurrent(jobs,
            [rc, state] () {
                state->painter->bltFixed(rc, state->dabsQueue);
            }
        );
    }

    state->allDirtyRects.append(rects);
}

std::pair<int, bool> KisBrushOp::doAsynchronousUpdate(QVector<KisRunnableStrokeJobData*> &jobs)
{
    bool someDabsAreStillInQueue = false;
    const bool hasPreparedDabsAtStart = m_dabExecutor->hasPreparedDabs();

    if (!m_updateSharedState && hasPreparedDabsAtStart) {

        m_updateSharedState = toQShared(new UpdateSharedState());
        UpdateSharedStateSP state = m_updateSharedState;

        state->painter = painter();

        {
            const qreal dabRenderingTime = m_dabExecutor->averageDabRenderingTime();
            const qreal totalRenderingTimePerDab = dabRenderingTime + m_avgUpdateTimePerDab.rollingMeanSafe();

            const int dabsLimit =
                totalRenderingTimePerDab > 0 ?
                    qMax(10, int(m_maxUpdatePeriod  / totalRenderingTimePerDab * m_idealNumRects)) :
                    -1;

            state->dabsQueue = m_dabExecutor->takeReadyDabs(painter()->hasMirroring(), dabsLimit, &someDabsAreStillInQueue);
        }

        KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(!state->dabsQueue.isEmpty(),
                                             std::make_pair(m_currentUpdatePeriod, false));

        const int diameter = m_dabExecutor->averageDabSize();
        const qreal spacing = m_avgSpacing.rollingMean();

        const int idealNumRects = m_idealNumRects;

        QVector<QRect> rects;

        if (painter()->device()->defaultBounds()->wrapAroundMode()) {

            const QRect wrapRect = painter()->device()->defaultBounds()->imageBorderRect();
            const WrapAroundAxis wrapAroundModeAxis = painter()->device()->defaultBounds()->wrapAroundModeAxis();

            QList<KisRenderedDab> wrappedDabs;

            Q_FOREACH (const KisRenderedDab &dab, state->dabsQueue) {
                const QVector<QPoint> normalizationOrigins =
                    KisWrappedRect::normalizationOriginsForRect(dab.realBounds(), wrapRect, wrapAroundModeAxis);

                Q_FOREACH(const QPoint &pt, normalizationOrigins) {
                    KisRenderedDab newDab = dab;

                    newDab.offset = pt;
                    rects.append(KisWrappedRect::clipToWrapRect(newDab.realBounds(), wrapRect, wrapAroundModeAxis));
                    wrappedDabs.append(newDab);
                }
            }

            state->dabsQueue = wrappedDabs;

        } else {
            Q_FOREACH (const KisRenderedDab &dab, state->dabsQueue) {
                rects.append(dab.realBounds());
            }
        }

        rects = KisPaintOpUtils::splitDabsIntoRects(rects,
                                                    idealNumRects, diameter, spacing);

        state->allDirtyRects = rects;

        Q_FOREACH (const KisRenderedDab &dab, state->dabsQueue) {
            state->dabPoints.append(dab.realBounds().center());
        }

        state->dabRenderingTimer.start();

        Q_FOREACH (const QRect &rc, rects) {
            KritaUtils::addJobConcurrent(jobs,
                [rc, state] () {
                    state->painter->bltFixed(rc, state->dabsQueue);
                }
            );
        }

        if (state->painter->hasHorizontalMirroring()) {
            addMirroringJobs(Qt::Horizontal, rects, state, jobs);
        }

        if (state->painter->hasVerticalMirroring()) {
            addMirroringJobs(Qt::Vertical, rects, state, jobs);
        }

        if (state->painter->hasHorizontalMirroring() && state->painter->hasVerticalMirroring()) {
            addMirroringJobs(Qt::Horizontal, rects, state, jobs);
        }

        KritaUtils::addJobSequential(jobs,
                [state, this, someDabsAreStillInQueue] () {
                    Q_FOREACH(const QRect &rc, state->allDirtyRects) {
                        state->painter->addDirtyRect(rc);
                    }

                    state->painter->setAverageOpacity(state->dabsQueue.last().averageOpacity);

                    const int updateRenderingTime = state->dabRenderingTimer.elapsed();
                    const qreal dabRenderingTime = m_dabExecutor->averageDabRenderingTime();

                    m_avgNumDabs(state->dabsQueue.size());

                    const qreal currentUpdateTimePerDab = qreal(updateRenderingTime) / state->dabsQueue.size();
                    m_avgUpdateTimePerDab(currentUpdateTimePerDab);

                    const qreal totalRenderingTimePerDab = dabRenderingTime + currentUpdateTimePerDab;

                    const int approxDabRenderingTime =
                        qreal(totalRenderingTimePerDab) * m_avgNumDabs.rollingMean() / m_idealNumRects;

                    m_currentUpdatePeriod =
                        someDabsAreStillInQueue ? m_minUpdatePeriod :
                        qBound(m_minUpdatePeriod, int(1.5 * approxDabRenderingTime), m_maxUpdatePeriod);

                    {
                    }

                    state->dabsQueue.clear();

                    m_updateSharedState.clear();
                }
        );
    } else if (m_updateSharedState && hasPreparedDabsAtStart) {
        someDabsAreStillInQueue = true;
    }

    return std::make_pair(m_currentUpdatePeriod, someDabsAreStillInQueue);
}

KisSpacingInformation KisBrushOp::updateSpacingImpl(const KisPaintInformation &info) const
{
    const qreal scale = m_sizeOption.apply(info) * KisLodTransform::lodToScale(painter()->device());
    qreal rotation = m_rotationOption.apply(info);
    return effectiveSpacing(scale, rotation, &m_airbrushData, &m_spacingOption, info);
}

KisTimingInformation KisBrushOp::updateTimingImpl(const KisPaintInformation &info) const
{
    return KisPaintOpPluginUtils::effectiveTiming(&m_airbrushData, &m_rateOption, info);
}

void KisBrushOp::paintLine(const KisPaintInformation& pi1, const KisPaintInformation& pi2, KisDistanceInformation *currentDistance)
{
    if (m_sharpnessOption.isChecked() && m_brush && (m_brush->width() == 1) && (m_brush->height() == 1)) {

        if (!m_lineCacheDevice) {
            m_lineCacheDevice = source()->createCompositionSourceDevice();
        }
        else {
            m_lineCacheDevice->clear();
        }

        KisPainter p(m_lineCacheDevice);
        p.setPaintColor(painter()->paintColor());
        p.drawDDALine(pi1.pos(), pi2.pos());

        QRect rc = m_lineCacheDevice->extent();
        painter()->bitBlt(rc.x(), rc.y(), m_lineCacheDevice, rc.x(), rc.y(), rc.width(), rc.height());
    painter()->renderMirrorMask(rc, m_lineCacheDevice);
    }
    else {
        KisPaintOp::paintLine(pi1, pi2, currentDistance);
    }
}
