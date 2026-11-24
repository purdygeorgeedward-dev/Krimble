/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_freehand_helper.h"

#include <QTimer>
#include <QElapsedTimer>
#include <QQueue>

#include <klocalizedstring.h>

#include <KoPointerEvent.h>
#include <KoCanvasResourceProvider.h>

#include "kis_algebra_2d.h"
#include "kis_distance_information.h"
#include "kis_painting_information_builder.h"
#include "kis_image.h"
#include "kis_painter.h"
#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_utils.h>

#include "kis_update_time_monitor.h"
#include "kis_stabilized_events_sampler.h"
#include "KisStabilizerDelayedPaintHelper.h"
#include "kis_config.h"

#include "kis_random_source.h"
#include "KisPerStrokeRandomSource.h"

#include "strokes/freehand_stroke.h"
#include "strokes/KisFreehandStrokeInfo.h"
#include "KisAsynchronousStrokeUpdateHelper.h"
#include "kis_canvas_resource_provider.h"
#include <KisOptimizedBrushOutline.h>

#include <math.h>

const qreal AIRBRUSH_INTERVAL_FACTOR = 0.5;

const qreal SPACING_UPDATE_INTERVAL = 50.0;

const qreal TIMING_UPDATE_INTERVAL = 50.0;

struct KisToolFreehandHelper::Private
{
    KoCanvasResourceProvider *resourceManager;
    KisPaintingInformationBuilder *infoBuilder;
    KisStrokesFacade *strokesFacade;
    KisAsynchronousStrokeUpdateHelper asyncUpdateHelper;

    KUndo2MagicString transactionText;

    bool haveTangent;
    QPointF previousTangent;

    bool hasPaintAtLeastOnce;

    QElapsedTimer strokeTime;
    QTimer strokeTimeoutTimer;

    QVector<KisFreehandStrokeInfo*> strokeInfos;
    KisResourcesSnapshotSP resources;
    KisStrokeId strokeId;

    KisPaintInformation previousPaintInformation;
    KisPaintInformation olderPaintInformation;
    KisPaintInformation tentativePaintInformation;

    QPoint lastDrawnPixel;
    QPoint tentativePixel;
    bool hasLastDrawnPixel;
    bool hasTentativePixel;

    KisSmoothingOptionsSP smoothingOptions;

    KisRandomSourceSP fakeDabRandomSource;
    KisPerStrokeRandomSourceSP fakeStrokeRandomSource;

    QTimer airbrushingTimer;

    QList<KisPaintInformation> history;
    QList<qreal> distanceHistory;

    KisPaintOpUtils::PositionHistory lastCursorPos;

    bool usingStabilizer;
    QQueue<KisPaintInformation> stabilizerDeque;
    QTimer stabilizerPollTimer;
    KisStabilizedEventsSampler stabilizedSampler;
    KisStabilizerDelayedPaintHelper stabilizerDelayedPaintHelper;

    qreal effectiveSmoothnessDistance(qreal speed) const;

    bool isTentativePixel(const QPoint &currentPixelPos) const
    {
        return abs(currentPixelPos.x() - lastDrawnPixel.x()) <= 1 && abs(currentPixelPos.y() - lastDrawnPixel.y()) <= 1;
    }

    static QPoint toPixelPos(const QPointF &pos)
    {
        return QPoint(qFloor(pos.x()), qFloor(pos.y()));
    }

    static void setPaintInfoPixelPos(KisPaintInformation &info, const QPoint &pixelPos)
    {
        info.setPos(QPointF(qreal(pixelPos.x()) + 0.5, qreal(pixelPos.y()) + 0.5));
    }
};

KisToolFreehandHelper::KisToolFreehandHelper(KisPaintingInformationBuilder *infoBuilder,
                                             KoCanvasResourceProvider *resourceManager,
                                             const KUndo2MagicString &transactionText,
                                             KisSmoothingOptions *smoothingOptions)
    : m_d(new Private())
{
    m_d->resourceManager = resourceManager;
    m_d->infoBuilder = infoBuilder;
    m_d->transactionText = transactionText;
    m_d->smoothingOptions = KisSmoothingOptionsSP(
                smoothingOptions ? smoothingOptions : new KisSmoothingOptions());

    m_d->fakeDabRandomSource = new KisRandomSource();
    m_d->fakeStrokeRandomSource = new KisPerStrokeRandomSource();

    m_d->strokeTimeoutTimer.setSingleShot(true);
    connect(&m_d->strokeTimeoutTimer, SIGNAL(timeout()), SLOT(finishStroke()));
    connect(&m_d->airbrushingTimer, SIGNAL(timeout()), SLOT(doAirbrushing()));
    connect(&m_d->stabilizerPollTimer, SIGNAL(timeout()), SLOT(stabilizerPollAndPaint()));
    connect(m_d->smoothingOptions.data(), SIGNAL(sigSmoothingTypeChanged()), SLOT(slotSmoothingTypeChanged()));

    m_d->stabilizerDelayedPaintHelper.setPaintLineCallback(
                [this](const KisPaintInformation &pi1, const KisPaintInformation &pi2) {
                    paintLine(pi1, pi2);
                });
    m_d->stabilizerDelayedPaintHelper.setUpdateOutlineCallback(
                [this]() {
                    Q_EMIT requestExplicitUpdateOutline();
                });
}

KisToolFreehandHelper::~KisToolFreehandHelper()
{
    delete m_d;
}

void KisToolFreehandHelper::setSmoothness(KisSmoothingOptionsSP smoothingOptions)
{
    m_d->smoothingOptions = smoothingOptions;
}

KisSmoothingOptionsSP KisToolFreehandHelper::smoothingOptions() const
{
    return m_d->smoothingOptions;
}

KisOptimizedBrushOutline KisToolFreehandHelper::paintOpOutline(const QPointF &savedCursorPos,
                                                               const KoPointerEvent *event,
                                                               const KisPaintOpSettingsSP globalSettings,
                                                               KisPaintOpSettings::OutlineMode mode) const
{
    KisPaintOpSettingsSP settings = globalSettings;
    QPointF prevPoint = m_d->lastCursorPos.pushThroughHistory(savedCursorPos, currentZoom());
    qreal startAngle = KisAlgebra2D::directionBetweenPoints(prevPoint, savedCursorPos, 0);
    KisDistanceInformation distanceInfo(prevPoint, startAngle);

    KisPaintInformation info;

    if (!m_d->strokeInfos.isEmpty()) {
        settings = m_d->resources->currentPaintOpPreset()->settings();
        if (m_d->stabilizerDelayedPaintHelper.running() &&
                m_d->stabilizerDelayedPaintHelper.hasLastPaintInformation()) {
            info = m_d->stabilizerDelayedPaintHelper.lastPaintInformation();
        } else {
            info = m_d->previousPaintInformation;
        }

        KisDistanceInformation *buddyDistance =
            m_d->strokeInfos.first()->buddyDragDistance();

        if (buddyDistance) {
            distanceInfo = *buddyDistance;
            distanceInfo.overrideLastValues(prevPoint, startAngle);

        } else if (m_d->strokeInfos.first()->dragDistance->isStarted()) {
            distanceInfo = *m_d->strokeInfos.first()->dragDistance;
        }
    } else {
        info = m_d->infoBuilder->hover(savedCursorPos, event, !m_d->strokeInfos.isEmpty());
    }

    KisPaintInformation::DistanceInformationRegistrar registrar =
        info.registerDistanceInformation(&distanceInfo);

    info.setRandomSource(m_d->fakeDabRandomSource);
    info.setPerStrokeRandomSource(m_d->fakeStrokeRandomSource);

    KisOptimizedBrushOutline outline = settings->brushOutline(info, mode, currentPhysicalZoom());

    if (m_d->resources &&
        m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::STABILIZER &&
        m_d->smoothingOptions->useDelayDistance()) {

        const qreal R = m_d->smoothingOptions->delayDistance() /
            m_d->resources->effectiveZoom();

        outline.addEllipse(info.pos(), R, R);
    }

    return outline;
}

void KisToolFreehandHelper::cursorMoved(const QPointF &cursorPos)
{
    m_d->lastCursorPos.pushThroughHistory(cursorPos, currentZoom());
}

void KisToolFreehandHelper::initPaint(KoPointerEvent *event,
                                      const QPointF &pixelCoords,
                                      KisImageWSP image, KisNodeSP currentNode,
                                      KisStrokesFacade *strokesFacade,
                                      KisNodeSP overrideNode,
                                      KisDefaultBoundsBaseSP bounds)
{
    if (event->isTouchEvent()) {
        m_d->lastCursorPos.reset(pixelCoords);
        m_d->infoBuilder->reset();
    }

    QPointF prevPoint = m_d->lastCursorPos.pushThroughHistory(pixelCoords, currentZoom());
    m_d->strokeTime.start();
    KisPaintInformation pi =
        m_d->infoBuilder->startStroke(event, elapsedStrokeTime(), m_d->resourceManager);
    qreal startAngle = KisAlgebra2D::directionBetweenPoints(prevPoint, pixelCoords, 0.0);

    initPaintImpl(startAngle,
                  pi,
                  m_d->resourceManager,
                  image,
                  currentNode,
                  strokesFacade,
                  overrideNode,
                  bounds);
}

bool KisToolFreehandHelper::isRunning() const
{
    return bool(m_d->strokeId);
}

void KisToolFreehandHelper::initPaintImpl(qreal startAngle,
                                          const KisPaintInformation &pi,
                                          KoCanvasResourceProvider *resourceManager,
                                          KisImageWSP image,
                                          KisNodeSP currentNode,
                                          KisStrokesFacade *strokesFacade,
                                          KisNodeSP overrideNode,
                                          KisDefaultBoundsBaseSP bounds)
{
    m_d->strokesFacade = strokesFacade;

    m_d->haveTangent = false;
    m_d->previousTangent = QPointF();

    m_d->hasPaintAtLeastOnce = false;

    m_d->previousPaintInformation = pi;

    m_d->resources = new KisResourcesSnapshot(image,
                                              currentNode,
                                              resourceManager,
                                              bounds);
    if(overrideNode) {
        m_d->resources->setCurrentNode(overrideNode);
    }

    const bool airbrushing = m_d->resources->needsAirbrushing();
    const bool useSpacingUpdates = m_d->resources->needsSpacingUpdates();

    KisDistanceInitInfo startDistInfo(m_d->previousPaintInformation.pos(),
                                      startAngle,
                                      useSpacingUpdates ? SPACING_UPDATE_INTERVAL : LONG_TIME,
                                      airbrushing ? TIMING_UPDATE_INTERVAL : LONG_TIME,
                                      0);
    KisDistanceInformation startDist = startDistInfo.makeDistInfo();

    createPainters(m_d->strokeInfos,
                   startDist);

    KisStrokeStrategy *stroke =
        new FreehandStrokeStrategy(m_d->resources,
                                   m_d->strokeInfos,
                                   m_d->transactionText,
                                   FreehandStrokeStrategy::SupportsContinuedInterstrokeData |
                                   FreehandStrokeStrategy::SupportsTimedMergeId);

    m_d->strokeId = m_d->strokesFacade->startStroke(stroke);

    m_d->history.clear();
    m_d->distanceHistory.clear();
    m_d->hasLastDrawnPixel = false;
    m_d->hasTentativePixel = false;

    if (airbrushing) {
        m_d->airbrushingTimer.setInterval(computeAirbrushTimerInterval());
        m_d->airbrushingTimer.start();
    } else if (m_d->resources->presetNeedsAsynchronousUpdates()) {
        m_d->asyncUpdateHelper.startUpdateStream(m_d->strokesFacade, m_d->strokeId);
    }

    if (m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::STABILIZER) {
        stabilizerStart(m_d->previousPaintInformation);
    }

    if (airbrushing) {
        paintAt(pi);
    }
}

KoCanvasResourceProvider *KisToolFreehandHelper::resourceManager() const
{
    return m_d->resourceManager;
}

void KisToolFreehandHelper::paintBezierSegment(KisPaintInformation pi1, KisPaintInformation pi2,
                                               QPointF tangent1, QPointF tangent2)
{
    if (tangent1.isNull() || tangent2.isNull()) return;

    const qreal maxSanePoint = 1e6;

    QPointF controlTarget1;
    QPointF controlTarget2;

    QPointF controlDirection1 = pi1.pos() + tangent1;
    QPointF controlDirection2 = pi2.pos() - tangent2;

    QLineF line1(pi1.pos(), controlDirection1);
    QLineF line2(pi2.pos(), controlDirection2);

    QLineF line3(controlDirection1, controlDirection2);
    QLineF line4(pi1.pos(), pi2.pos());

    QPointF intersection;
    if (line3.intersects(line4, &intersection) == QLineF::BoundedIntersection) {
        qreal controlLength = line4.length() / 2;

        line1.setLength(controlLength);
        line2.setLength(controlLength);

        controlTarget1 = line1.p2();
        controlTarget2 = line2.p2();
    } else {
        QLineF::IntersectType type = line1.intersects(line2, &intersection);

        if (type == QLineF::NoIntersection ||
            intersection.manhattanLength() > maxSanePoint) {

            intersection = 0.5 * (pi1.pos() + pi2.pos());
        }

        controlTarget1 = intersection;
        controlTarget2 = intersection;
    }

    qreal coeff = 0.8;

    qreal velocity1 = QLineF(QPointF(), tangent1).length();
    qreal velocity2 = QLineF(QPointF(), tangent2).length();

    if (velocity1 == 0.0 || velocity2 == 0.0) {
        velocity1 = 1e-6;
        velocity2 = 1e-6;
        warnKrita << "WARNING: Basic Smoothing: Velocity is Zero! Please report a bug:" << ppVar(velocity1) << ppVar(velocity2);
    }

    qreal similarity = qMin(velocity1/velocity2, velocity2/velocity1);

    similarity = qMax(similarity, qreal(0.5));

    coeff *= 1 - qMax(qreal(0.0), similarity - qreal(0.8));

    Q_ASSERT(coeff > 0);

    QPointF control1;
    QPointF control2;

    if (velocity1 > velocity2) {
        control1 = pi1.pos() * (1.0 - coeff) + coeff * controlTarget1;
        coeff *= similarity;
        control2 = pi2.pos() * (1.0 - coeff) + coeff * controlTarget2;
    } else {
        control2 = pi2.pos() * (1.0 - coeff) + coeff * controlTarget2;
        coeff *= similarity;
        control1 = pi1.pos() * (1.0 - coeff) + coeff * controlTarget1;
    }

    paintBezierCurve(pi1,
                     control1,
                     control2,
                     pi2);
}

qreal KisToolFreehandHelper::Private::effectiveSmoothnessDistance(qreal speed) const
{
    qreal zoomingCoeff = 1.0;

    if ((smoothingOptions->smoothingType() == KisSmoothingOptions::STABILIZER) ^
         smoothingOptions->useScalableDistance()) {

        zoomingCoeff = 1.0 / resources->effectiveZoom();
    }

    return zoomingCoeff * ((1.0 - speed) * smoothingOptions->smoothnessDistanceMax() + speed * smoothingOptions->smoothnessDistanceMin());
}

void KisToolFreehandHelper::paintEvent(KoPointerEvent *event)
{
    KisPaintInformation info =
            m_d->infoBuilder->continueStroke(event,
                                             elapsedStrokeTime());
    KisUpdateTimeMonitor::instance()->reportMouseMove(info.pos());

    paint(info);
}

void KisToolFreehandHelper::paint(KisPaintInformation &info)
{

    KisPaintInformation lastUsedPaintInformation;

    if (m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::WEIGHTED_SMOOTHING
        && (m_d->smoothingOptions->smoothnessDistanceMin() > 0.0
            || m_d->smoothingOptions->smoothnessDistanceMax() > 0.0)) {

        {
            QPointF prevPos;

            if (!m_d->history.isEmpty()) {
                const KisPaintInformation &prevPi = m_d->history.last();
                prevPos = prevPi.pos();
            } else {
                prevPos = m_d->previousPaintInformation.pos();
            }

            qreal currentDistance = QVector2D(info.pos() - prevPos).length();
            m_d->distanceHistory.append(currentDistance);
        }

        m_d->history.append(info);

        qreal x = 0.0;
        qreal y = 0.0;

        if (m_d->history.size() > 3) {
            const qreal sigma = m_d->effectiveSmoothnessDistance(m_d->previousPaintInformation.drawingSpeed()) / 3.0;

            qreal gaussianWeight = 1 / (sqrt(2 * M_PI) * sigma);
            qreal gaussianWeight2 = sigma * sigma;
            qreal distanceSum = 0.0;
            qreal scaleSum = 0.0;
            qreal pressure = 0.0;
            qreal baseRate = 0.0;

            Q_ASSERT(m_d->history.size() == m_d->distanceHistory.size());

            for (int i = m_d->history.size() - 1; i >= 0; i--) {
                qreal rate = 0.0;

                const KisPaintInformation nextInfo = m_d->history.at(i);
                double distance = m_d->distanceHistory.at(i);
                Q_ASSERT(distance >= 0.0);

                qreal pressureGrad = 0.0;
                if (i < m_d->history.size() - 1) {
                    pressureGrad = nextInfo.pressure() - m_d->history.at(i + 1).pressure();

                    const qreal tailAggressiveness = 40.0 * m_d->smoothingOptions->tailAggressiveness();

                    if (pressureGrad > 0.0 ) {
                        pressureGrad *= tailAggressiveness * (1.0 - nextInfo.pressure());
                        distance += pressureGrad * 3.0 * sigma;
                    }
                }

                if (gaussianWeight2 != 0.0) {
                    distanceSum += distance;
                    rate = gaussianWeight * exp(-distanceSum * distanceSum / (2 * gaussianWeight2));
                }

                if (m_d->history.size() - i == 1) {
                    baseRate = rate;
                } else if (baseRate / rate > 100) {
                    break;
                }

                scaleSum += rate;
                x += rate * nextInfo.pos().x();
                y += rate * nextInfo.pos().y();

                if (m_d->smoothingOptions->smoothPressure()) {
                    pressure += rate * nextInfo.pressure();
                }
            }

            if (scaleSum != 0.0) {
                x /= scaleSum;
                y /= scaleSum;

                if (m_d->smoothingOptions->smoothPressure()) {
                    pressure /= scaleSum;
                }
            }

            if ((x != 0.0 && y != 0.0) || (x == info.pos().x() && y == info.pos().y())) {
                info.setPos(QPointF(x, y));
                if (m_d->smoothingOptions->smoothPressure()) {
                    info.setPressure(pressure);
                }
                m_d->history.last() = info;
            }
        }
    }

    if (m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::SIMPLE_SMOOTHING
            || m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::WEIGHTED_SMOOTHING)
        {
            if (!m_d->haveTangent) {
                m_d->haveTangent = true;
                m_d->previousTangent =
                    (info.pos() - m_d->previousPaintInformation.pos()) /
                    qMax(qreal(1.0), info.currentTime() - m_d->previousPaintInformation.currentTime());
            } else {
                QPointF newTangent = (info.pos() - m_d->olderPaintInformation.pos()) /
                    qMax(qreal(1.0), info.currentTime() - m_d->olderPaintInformation.currentTime());

                if (newTangent.isNull() || m_d->previousTangent.isNull())
                {
                    paintLine(m_d->previousPaintInformation, info);
                } else {
                    paintBezierSegment(m_d->olderPaintInformation, m_d->previousPaintInformation,
                                    m_d->previousTangent, newTangent);
                }

                m_d->previousTangent = newTangent;
            }
            m_d->olderPaintInformation = m_d->previousPaintInformation;

            if (!m_d->airbrushingTimer.isActive()) {
                m_d->strokeTimeoutTimer.start(100);
            }
        }

    else if (m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::NO_SMOOTHING){
        paintLine(m_d->previousPaintInformation, info);
    }

    if (m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::PIXEL_PERFECT) {
        QPoint currentPixelPos = Private::toPixelPos(info.pos());

        if (!m_d->hasLastDrawnPixel) {
            Private::setPaintInfoPixelPos(info, currentPixelPos);
            paintLine(info, info);
            m_d->lastDrawnPixel = currentPixelPos;
            m_d->hasLastDrawnPixel = true;
            m_d->hasTentativePixel = false;
            m_d->olderPaintInformation = info;

        } else {
            bool lastTentative = m_d->isTentativePixel(currentPixelPos);
            if (m_d->hasTentativePixel && !lastTentative) {
                Private::setPaintInfoPixelPos(m_d->tentativePaintInformation, m_d->tentativePixel);
                paintLine(m_d->olderPaintInformation, m_d->tentativePaintInformation);
                m_d->hasTentativePixel = false;
                m_d->lastDrawnPixel = m_d->tentativePixel;
                m_d->olderPaintInformation = m_d->tentativePaintInformation;
                lastTentative = m_d->isTentativePixel(currentPixelPos);
            }

            if (lastTentative) {
                m_d->hasTentativePixel = true;
                m_d->tentativePixel = currentPixelPos;
                m_d->tentativePaintInformation = info;
            } else {
                Private::setPaintInfoPixelPos(info, currentPixelPos);
                paintLine(m_d->olderPaintInformation, info);
                m_d->lastDrawnPixel = currentPixelPos;
                m_d->olderPaintInformation = info;
            }
        }
    }

    if (m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::STABILIZER) {
        m_d->stabilizedSampler.addEvent(info);
        if (m_d->stabilizerDelayedPaintHelper.running()) {
            m_d->stabilizerDelayedPaintHelper.paintSome();
        }
    } else {
        m_d->previousPaintInformation = info;
    }

    if(m_d->airbrushingTimer.isActive()) {
        m_d->airbrushingTimer.start();
    }
}

void KisToolFreehandHelper::endPaint()
{
    if (!m_d->hasPaintAtLeastOnce) {
        paintAt(m_d->previousPaintInformation);
    } else if (m_d->smoothingOptions->smoothingType() != KisSmoothingOptions::NO_SMOOTHING) {
        finishStroke();
    }
    m_d->strokeTimeoutTimer.stop();

    if(m_d->airbrushingTimer.isActive()) {
        m_d->airbrushingTimer.stop();
    }

    if (m_d->smoothingOptions->smoothingType() == KisSmoothingOptions::STABILIZER) {
        stabilizerEnd();
    }

    if (m_d->asyncUpdateHelper.isActive()) {
        m_d->asyncUpdateHelper.endUpdateStream();
    }

    m_d->strokeInfos.clear();

    m_d->strokesFacade->addJob(m_d->strokeId,
       new KisAsynchronousStrokeUpdateHelper::UpdateData(true));

    m_d->strokesFacade->endStroke(m_d->strokeId);
    m_d->strokeId.clear();
    m_d->infoBuilder->reset();
}

void KisToolFreehandHelper::cancelPaint()
{
    if (!m_d->strokeId) return;

    m_d->strokeTimeoutTimer.stop();

    if (m_d->airbrushingTimer.isActive()) {
        m_d->airbrushingTimer.stop();
    }

    if (m_d->asyncUpdateHelper.isActive()) {
        m_d->asyncUpdateHelper.cancelUpdateStream();
    }

    if (m_d->stabilizerPollTimer.isActive()) {
        m_d->stabilizerPollTimer.stop();
    }

    if (m_d->stabilizerDelayedPaintHelper.running()) {
        m_d->stabilizerDelayedPaintHelper.cancel();
    }

    m_d->strokeInfos.clear();

    m_d->strokesFacade->cancelStroke(m_d->strokeId);
    m_d->strokeId.clear();

}

int KisToolFreehandHelper::elapsedStrokeTime() const
{
    return m_d->strokeTime.elapsed();
}

void KisToolFreehandHelper::stabilizerStart(KisPaintInformation firstPaintInfo)
{
    m_d->usingStabilizer = true;
    int sampleSize = qRound(m_d->effectiveSmoothnessDistance(firstPaintInfo.drawingSpeed()));
    sampleSize = qMax(3, sampleSize);

    m_d->stabilizerDeque.clear();
    for (int i = sampleSize; i > 0; i--) {
        m_d->stabilizerDeque.enqueue(firstPaintInfo);
    }

    KisConfig cfg(true);
    int stabilizerSampleSize = cfg.stabilizerSampleSize();
    m_d->stabilizerPollTimer.setInterval(stabilizerSampleSize);
    m_d->stabilizerPollTimer.start();

    bool delayedPaintEnabled = cfg.stabilizerDelayedPaint();
    if (delayedPaintEnabled) {
        m_d->stabilizerDelayedPaintHelper.start(firstPaintInfo);
    }

    m_d->stabilizedSampler.clear();
    m_d->stabilizedSampler.addEvent(firstPaintInfo);
}

KisPaintInformation
KisToolFreehandHelper::getStabilizedPaintInfo(const QQueue<KisPaintInformation> &queue,
                                              const KisPaintInformation &lastPaintInfo)
{
    KisPaintInformation result(lastPaintInfo.pos(),
                               lastPaintInfo.pressure(),
                               lastPaintInfo.xTilt(),
                               lastPaintInfo.yTilt(),
                               lastPaintInfo.rotation(),
                               lastPaintInfo.tangentialPressure(),
                               lastPaintInfo.perspective(),
                               elapsedStrokeTime(),
                               lastPaintInfo.drawingSpeed());

    result.setCanvasRotation(lastPaintInfo.canvasRotation());
    result.setCanvasMirroredH(lastPaintInfo.canvasMirroredH());
    result.setCanvasMirroredV(lastPaintInfo.canvasMirroredV());

    if (queue.size() > 1) {
        QQueue<KisPaintInformation>::const_iterator it = queue.constBegin();
        QQueue<KisPaintInformation>::const_iterator end = queue.constEnd();

        it++;
        int i = 2;

        if (m_d->smoothingOptions->stabilizeSensors()) {
            while (it != end) {
                qreal k = qreal(i - 1) / i;
                result.KisPaintInformation::mixOtherWithoutTime(k, *it);
                it++;
                i++;
            }
        } else{
            while (it != end) {
                qreal k = qreal(i - 1) / i;
                result.KisPaintInformation::mixOtherOnlyPosition(k, *it);
                it++;
                i++;
            }
        }
    }

    return result;
}

void KisToolFreehandHelper::stabilizerPollAndPaint()
{
    KisStabilizedEventsSampler::iterator it;
    KisStabilizedEventsSampler::iterator end;
    std::tie(it, end) = m_d->stabilizedSampler.range();
    QVector<KisPaintInformation> delayedPaintTodoItems;

    for (; it != end; ++it) {
        KisPaintInformation sampledInfo = *it;

        bool canPaint = true;

        if (m_d->smoothingOptions->useDelayDistance()) {
            const qreal R = m_d->smoothingOptions->delayDistance() /
                    m_d->resources->effectiveZoom();

            QPointF diff = sampledInfo.pos() - m_d->previousPaintInformation.pos();
            qreal dx = sqrt(pow2(diff.x()) + pow2(diff.y()));

            if (!(dx > R)) {
                if (m_d->resources->needsAirbrushing()) {
                    sampledInfo.setPos(m_d->previousPaintInformation.pos());
                }
                else {
                    canPaint = false;
                }
            }
        }

        if (canPaint) {
            KisPaintInformation newInfo = getStabilizedPaintInfo(m_d->stabilizerDeque, sampledInfo);

            if (m_d->stabilizerDelayedPaintHelper.running()) {
                delayedPaintTodoItems.append(newInfo);
            } else {
                paintLine(m_d->previousPaintInformation, newInfo);
            }
            m_d->previousPaintInformation = newInfo;

            m_d->stabilizerDeque.dequeue();
            m_d->stabilizerDeque.enqueue(sampledInfo);
        } else if (m_d->stabilizerDeque.head().pos() != m_d->previousPaintInformation.pos()) {
            QQueue<KisPaintInformation>::iterator it = m_d->stabilizerDeque.begin();
            QQueue<KisPaintInformation>::iterator end = m_d->stabilizerDeque.end();

            while (it != end) {
                *it = m_d->previousPaintInformation;
                ++it;
            }
        }
    }

    m_d->stabilizedSampler.clear();

    if (m_d->stabilizerDelayedPaintHelper.running()) {
        m_d->stabilizerDelayedPaintHelper.update(delayedPaintTodoItems);
    } else {
        Q_EMIT requestExplicitUpdateOutline();
    }
}

void KisToolFreehandHelper::stabilizerEnd()
{
    m_d->stabilizerPollTimer.stop();

    if (m_d->smoothingOptions->finishStabilizedCurve()) {
        stabilizerPollAndPaint();

        m_d->stabilizedSampler.addFinishingEvent(m_d->stabilizerDeque.size());
        stabilizerPollAndPaint();
    }

    if (m_d->stabilizerDelayedPaintHelper.running()) {
        m_d->stabilizerDelayedPaintHelper.end();
    }
    m_d->usingStabilizer = false;
}

void KisToolFreehandHelper::slotSmoothingTypeChanged()
{
    if (!isRunning()) {
        return;
    }
    KisSmoothingOptions::SmoothingType currentSmoothingType =
            m_d->smoothingOptions->smoothingType();
    if (m_d->usingStabilizer
            && (currentSmoothingType != KisSmoothingOptions::STABILIZER)) {
        stabilizerEnd();
    } else if (!m_d->usingStabilizer
            && (currentSmoothingType == KisSmoothingOptions::STABILIZER)) {
        stabilizerStart(m_d->previousPaintInformation);
    }
}

void KisToolFreehandHelper::finishStroke()
{
    if (m_d->haveTangent) {
        m_d->haveTangent = false;

        QPointF newTangent = (m_d->previousPaintInformation.pos() - m_d->olderPaintInformation.pos()) /
            qMax(qreal(1.0), m_d->previousPaintInformation.currentTime() - m_d->olderPaintInformation.currentTime());

        paintBezierSegment(m_d->olderPaintInformation,
                           m_d->previousPaintInformation,
                           m_d->previousTangent,
                           newTangent);
    }

}

void KisToolFreehandHelper::doAirbrushing()
{
    if (!m_d->strokeInfos.isEmpty()) {

        const KisPaintInformation &prevPaint = m_d->previousPaintInformation;
        KisPaintInformation nextPaint(prevPaint.pos(),
                                      prevPaint.pressure(),
                                      prevPaint.xTilt(),
                                      prevPaint.yTilt(),
                                      prevPaint.rotation(),
                                      prevPaint.tangentialPressure(),
                                      prevPaint.perspective(),
                                      elapsedStrokeTime(),
                                      0.0);
        nextPaint.setCanvasRotation(prevPaint.canvasRotation());
        nextPaint.setCanvasMirroredH(prevPaint.canvasMirroredH());
        nextPaint.setCanvasMirroredV(prevPaint.canvasMirroredV());
        paint(nextPaint);
    }
}

int KisToolFreehandHelper::computeAirbrushTimerInterval() const
{
    qreal realInterval = m_d->resources->airbrushingInterval() * AIRBRUSH_INTERVAL_FACTOR;
    return qMax(1, qFloor(realInterval));
}

qreal KisToolFreehandHelper::currentZoom() const
{
    return m_d->resourceManager ? m_d->resourceManager->resource(KoCanvasResource::EffectiveZoom).toReal() : 1.0;
}

qreal KisToolFreehandHelper::currentPhysicalZoom() const
{
    return m_d->resourceManager ? m_d->resourceManager->resource(KoCanvasResource::EffectivePhysicalZoom).toReal() : 1.0;
}

void KisToolFreehandHelper::paintAt(int strokeInfoId,
                                    const KisPaintInformation &pi)
{
    m_d->hasPaintAtLeastOnce = true;
    m_d->strokesFacade->addJob(m_d->strokeId,
                               new FreehandStrokeStrategy::Data(strokeInfoId, pi));

}

void KisToolFreehandHelper::paintLine(int strokeInfoId,
                                      const KisPaintInformation &pi1,
                                      const KisPaintInformation &pi2)
{
    m_d->hasPaintAtLeastOnce = true;
    m_d->strokesFacade->addJob(m_d->strokeId,
                               new FreehandStrokeStrategy::Data(strokeInfoId, pi1, pi2));

}

void KisToolFreehandHelper::paintBezierCurve(int strokeInfoId,
                                             const KisPaintInformation &pi1,
                                             const QPointF &control1,
                                             const QPointF &control2,
                                             const KisPaintInformation &pi2)
{

#ifdef DEBUG_BEZIER_CURVES
    KisPaintInformation tpi1;
    KisPaintInformation tpi2;

    tpi1 = pi1;
    tpi2 = pi2;

    tpi1.setPressure(0.3);
    tpi2.setPressure(0.3);

    paintLine(tpi1, tpi2);

    tpi1.setPressure(0.6);
    tpi2.setPressure(0.3);

    tpi1.setPos(pi1.pos());
    tpi2.setPos(control1);
    paintLine(tpi1, tpi2);

    tpi1.setPos(pi2.pos());
    tpi2.setPos(control2);
    paintLine(tpi1, tpi2);
#endif

    m_d->hasPaintAtLeastOnce = true;
    m_d->strokesFacade->addJob(m_d->strokeId,
                               new FreehandStrokeStrategy::Data(strokeInfoId,
                                                                pi1, control1, control2, pi2));

}

void KisToolFreehandHelper::createPainters(QVector<KisFreehandStrokeInfo*> &strokeInfos,
                                           const KisDistanceInformation &startDist)
{
    strokeInfos << new KisFreehandStrokeInfo(startDist);
}

void KisToolFreehandHelper::paintAt(const KisPaintInformation &pi)
{
    paintAt(0, pi);
}

void KisToolFreehandHelper::paintLine(const KisPaintInformation &pi1,
                                      const KisPaintInformation &pi2)
{
    paintLine(0, pi1, pi2);
}

void KisToolFreehandHelper::paintBezierCurve(const KisPaintInformation &pi1,
                                             const QPointF &control1,
                                             const QPointF &control2,
                                             const KisPaintInformation &pi2)
{
    paintBezierCurve(0, pi1, control1, control2, pi2);
}
