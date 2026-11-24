/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "kis_prescaled_projection.h"

#include <math.h>

#include <QImage>
#include <QColor>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QPainter>

#include <KoColorProfile.h>
#include <KoViewConverter.h>

#include "kis_config.h"
#include "kis_image_config.h"
#include "kis_config_notifier.h"
#include "kis_image.h"
#include "krita_utils.h"

#include "kis_coordinates_converter.h"
#include "kis_projection_backend.h"
#include "kis_image_pyramid.h"
#include "kis_display_filter.h"
#include <KisDisplayConfig.h>

#include <KisCanvasState.h>

namespace {
struct RelevantCanvasState
{
    qreal zoom = 1.0;
    qreal rotation = 0.0;
    QPointF viewportOffsetF;

    std::tuple<qreal,qreal> transformations() const {
        return {zoom, rotation};
    }

    bool operator==(const RelevantCanvasState& other) const {
        return qFuzzyCompare(zoom, other.zoom) &&
               qFuzzyCompare(rotation, other.rotation) &&
               viewportOffsetF == other.viewportOffsetF;
    }

    bool operator!=(const RelevantCanvasState& other) const {
        return !(*this == other);
    }

    static RelevantCanvasState fromCanvasState(const KisCanvasState &state) {
        return {state.effectiveZoom, state.rotation, state.viewportOffsetF};
    }
};
}

#define ceiledSize(sz) QSize(ceil((sz).width()), ceil((sz).height()))

inline void copyQImageBuffer(uchar* dst, const uchar* src , qint32 deltaX, qint32 width)
{
    if (deltaX >= 0) {
        memcpy(dst + 4 * deltaX, src, 4 *(width - deltaX) * sizeof(uchar));
    } else {
        memcpy(dst, src - 4 * deltaX, 4 *(width + deltaX) * sizeof(uchar));
    }
}

void copyQImage(qint32 deltaX, qint32 deltaY, QImage* dstImage, const QImage& srcImage)
{
    qint32 height = dstImage->height();
    qint32 width = dstImage->width();
    Q_ASSERT(dstImage->width() == srcImage.width() && dstImage->height() == srcImage.height());
    if (deltaY >= 0) {
        for (int y = 0; y < height - deltaY; y ++) {
            const uchar* src = srcImage.scanLine(y);
            uchar* dst = dstImage->scanLine(y + deltaY);
            copyQImageBuffer(dst, src, deltaX, width);
        }
    } else {
        for (int y = 0; y < height + deltaY; y ++) {
            const uchar* src = srcImage.scanLine(y - deltaY);
            uchar* dst = dstImage->scanLine(y);
            copyQImageBuffer(dst, src, deltaX, width);
        }
    }
}

struct KisPrescaledProjection::Private {
    Private()
        : viewportSize(0, 0)
        , projectionBackend(0) {
    }

    QImage prescaledQImage;

    std::optional<RelevantCanvasState> currentRelevantCanvasState;
    QSize updatePatchSize;
    QSize canvasSize;
    QSize viewportSize;
    KisImageWSP image;
    KisCoordinatesConverter *coordinatesConverter {0};
    KisProjectionBackend *projectionBackend {0};
};

KisPrescaledProjection::KisPrescaledProjection()
        : QObject(0)
        , m_d(new Private())
{
    updateSettings();

    m_d->projectionBackend = new KisImagePyramid(1);

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), SLOT(updateSettings()));
}

KisPrescaledProjection::~KisPrescaledProjection()
{
    delete m_d->projectionBackend;
    delete m_d;
}

void KisPrescaledProjection::setImage(KisImageWSP image)
{
    Q_ASSERT(image);
    m_d->image = image;
    m_d->projectionBackend->setImage(image);
}

QImage KisPrescaledProjection::prescaledQImage() const
{
    return m_d->prescaledQImage;
}

void KisPrescaledProjection::setCoordinatesConverter(KisCoordinatesConverter *coordinatesConverter)
{
    m_d->coordinatesConverter = coordinatesConverter;
    m_d->currentRelevantCanvasState = RelevantCanvasState::fromCanvasState(KisCanvasState::fromConverter(*coordinatesConverter));
}

void KisPrescaledProjection::updateSettings()
{
    KisImageConfig imageConfig(false);
    m_d->updatePatchSize.setWidth(imageConfig.updatePatchWidth());
    m_d->updatePatchSize.setHeight(imageConfig.updatePatchHeight());
}

void KisPrescaledProjection::notifyCanvasStateChanged(const KisCanvasState &state)
{
    auto relevantState = RelevantCanvasState::fromCanvasState(state);

    if (m_d->currentRelevantCanvasState == relevantState) return;

    if (!m_d->currentRelevantCanvasState ||
        m_d->currentRelevantCanvasState->transformations() != relevantState.transformations()) {

        updateViewportSize();
        preScale();
    } else {
        const QPointF moveOffset = m_d->currentRelevantCanvasState->viewportOffsetF - relevantState.viewportOffsetF;
        viewportMoved(-moveOffset);
    }

    m_d->currentRelevantCanvasState = relevantState;
}

void KisPrescaledProjection::viewportMoved(const QPointF &offset)
{
    if (m_d->prescaledQImage.isNull()) return;
    if (offset.isNull()) return;

    QPoint alignedOffset = offset.toPoint();

    if(offset != alignedOffset) {
        dbgRender << "prescaling the entire image because the offset is float";
        preScale();
        return;
    }

    QImage newImage = QImage(m_d->viewportSize, QImage::Format_ARGB32);
    newImage.fill(0);

    QRect newViewportRect = QRect(QPoint(0,0), m_d->viewportSize);
    QRect oldViewportRect = newViewportRect.translated(alignedOffset);

    QRegion updateRegion = newViewportRect;
    QRect savedArea = newViewportRect & oldViewportRect;
    if(!savedArea.isEmpty()) {
        copyQImage(alignedOffset.x(), alignedOffset.y(), &newImage, m_d->prescaledQImage);
        updateRegion -= savedArea;
    }

    QPainter gc(&newImage);
    auto rc = updateRegion.begin();
    while (rc != updateRegion.end()) {
        QRect rect = *rc;
        QRect imageRect =
            m_d->coordinatesConverter->viewportToImage(rect).toAlignedRect();
        QVector<QRect> patches =
            KritaUtils::splitRectIntoPatches(imageRect, m_d->updatePatchSize);

        Q_FOREACH (const QRect& rc, patches) {
            QRect viewportPatch =
                m_d->coordinatesConverter->imageToViewport(rc).toAlignedRect();

            KisPPUpdateInfoSP info = getInitialUpdateInformation(QRect());
            fillInUpdateInformation(viewportPatch, info);
            drawUsingBackend(gc, info);
        }
        rc++;
    }

    m_d->prescaledQImage = newImage;
}

void KisPrescaledProjection::slotImageSizeChanged(qint32 w, qint32 h)
{
    m_d->projectionBackend->setImageSize(w, h);
    updateViewportSize();
}

KisUpdateInfoSP KisPrescaledProjection::updateCache(const QRect &dirtyImageRect)
{
    if (!m_d->image) {
        dbgRender.noquote() << "Calling updateCache without an image:" << kisBacktrace() << Qt::endl;
        return new KisPPUpdateInfo();
    }

    QRect croppedImageRect = dirtyImageRect & m_d->image->bounds();
    if (croppedImageRect.isEmpty()) return new KisPPUpdateInfo();

    KisPPUpdateInfoSP info = getInitialUpdateInformation(croppedImageRect);
    m_d->projectionBackend->updateCache(croppedImageRect);

    return info;
}

void KisPrescaledProjection::recalculateCache(KisUpdateInfoSP info)
{
    KisPPUpdateInfoSP ppInfo = dynamic_cast<KisPPUpdateInfo*>(info.data());
    if(!ppInfo) return;

    QRect rawViewRect =
        m_d->coordinatesConverter->
        imageToViewport(ppInfo->dirtyImageRectVar).toAlignedRect();

    fillInUpdateInformation(rawViewRect, ppInfo);

    m_d->projectionBackend->recalculateCache(ppInfo);

    if(!info->dirtyViewportRect().isEmpty())
        updateScaledImage(ppInfo);
}

void KisPrescaledProjection::preScale()
{
    if (!m_d->image) return;

    m_d->prescaledQImage.fill(0);

    QRect viewportRect(QPoint(0, 0), m_d->viewportSize);
    QRect imageRect =
        m_d->coordinatesConverter->viewportToImage(viewportRect).toAlignedRect();

    QVector<QRect> patches =
        KritaUtils::splitRectIntoPatches(imageRect, m_d->updatePatchSize);

    Q_FOREACH (const QRect& rc, patches) {
        QRect viewportPatch = m_d->coordinatesConverter->imageToViewport(rc).toAlignedRect();
        KisPPUpdateInfoSP info = getInitialUpdateInformation(QRect());
        fillInUpdateInformation(viewportPatch, info);
        QPainter gc(&m_d->prescaledQImage);
        gc.setCompositionMode(QPainter::CompositionMode_Source);
        drawUsingBackend(gc, info);
    }

}

void KisPrescaledProjection::setDisplayConfig(const KisDisplayConfig &config)
{
    m_d->projectionBackend->setMonitorProfile(config.profile, config.intent, config.conversionFlags);
}

void KisPrescaledProjection::setChannelFlags(const QBitArray &channelFlags)
{
    m_d->projectionBackend->setChannelFlags(channelFlags);
}

void KisPrescaledProjection::setDisplayFilter(QSharedPointer<KisDisplayFilter> displayFilter)
{
    m_d->projectionBackend->setDisplayFilter(displayFilter);
}

void KisPrescaledProjection::updateViewportSize()
{
    QRect imageRect = m_d->coordinatesConverter->imageRectInWidgetPixels().toAlignedRect();
    QSizeF minimalSize(qMin(imageRect.width(), m_d->canvasSize.width()),
                       qMin(imageRect.height(), m_d->canvasSize.height()));
    QRectF minimalRect(QPointF(0,0), minimalSize);

    m_d->viewportSize = m_d->coordinatesConverter->widgetToViewport(minimalRect).toAlignedRect().size();

    if (m_d->prescaledQImage.isNull() ||
        m_d->prescaledQImage.size() != m_d->viewportSize) {

        m_d->prescaledQImage = QImage(m_d->viewportSize, QImage::Format_ARGB32);
        m_d->prescaledQImage.fill(0);
    }
}

void KisPrescaledProjection::notifyCanvasSizeChanged(const QSize &widgetSize)
{
    m_d->canvasSize = widgetSize;
    updateViewportSize();
    preScale();
}

KisPPUpdateInfoSP KisPrescaledProjection::getInitialUpdateInformation(const QRect &dirtyImageRect)
{

    KisPPUpdateInfoSP info = new KisPPUpdateInfo();
    info->dirtyImageRectVar = dirtyImageRect;

    return info;
}

void KisPrescaledProjection::fillInUpdateInformation(const QRect &viewportRect,
                                                     KisPPUpdateInfoSP info)
{
    m_d->coordinatesConverter->imageScale(&info->scaleX, &info->scaleY);

    QRect croppedViewRect = viewportRect.intersected(QRect(QPoint(0, 0), m_d->viewportSize));

    info->imageRect = m_d->coordinatesConverter->viewportToImage(QRectF(croppedViewRect)).toAlignedRect();

    const int borderSize = BORDER_SIZE(qMax(info->scaleX, info->scaleY));
    info->imageRect.adjust(-borderSize, -borderSize, borderSize, borderSize);

    info->imageRect = info->imageRect & m_d->image->bounds();

    m_d->projectionBackend->alignSourceRect(info->imageRect, info->scaleX);

    info->viewportRect = m_d->coordinatesConverter->imageToViewport(info->imageRect);

    info->borderWidth = 0;
    if (SCALE_MORE_OR_EQUAL_TO(info->scaleX, info->scaleY, 1.0)) {
        if (SCALE_LESS_THAN(info->scaleX, info->scaleY, 2.0)) {
            dbgRender << "smoothBetween100And200Percent" << Qt::endl;
            info->renderHints = QPainter::SmoothPixmapTransform;
            info->borderWidth = borderSize;
        }
        info->transfer = KisPPUpdateInfo::DIRECT;
    } else {
        info->renderHints = QPainter::SmoothPixmapTransform;
        info->borderWidth = borderSize;
        info->transfer = KisPPUpdateInfo::PATCH;
    }

    dbgRender << "#####################################";
    dbgRender << ppVar(info->scaleX) << ppVar(info->scaleY);
    dbgRender << ppVar(info->borderWidth) << ppVar(info->renderHints);
    dbgRender << ppVar(info->transfer);
    dbgRender << ppVar(info->dirtyImageRectVar);
    dbgRender << "Not aligned rect of the canvas (raw):\t" << croppedViewRect;
    dbgRender << "Update rect in KisImage's pixels:\t" << info->imageRect;
    dbgRender << "Update rect in canvas' pixels:\t" << info->viewportRect;
    dbgRender << "#####################################";
}

void KisPrescaledProjection::updateScaledImage(KisPPUpdateInfoSP info)
{
    QPainter gc(&m_d->prescaledQImage);
    gc.setCompositionMode(QPainter::CompositionMode_Source);
    drawUsingBackend(gc, info);
}

void KisPrescaledProjection::drawUsingBackend(QPainter &gc, KisPPUpdateInfoSP info)
{
    if (info->imageRect.isEmpty()) return;

    if (info->transfer == KisPPUpdateInfo::DIRECT) {
        m_d->projectionBackend->drawFromOriginalImage(gc, info);
    } else {
        KisImagePatch patch = m_d->projectionBackend->getNearestPatch(info);
        patch.preScale(info->viewportRect);
        patch.drawMe(gc, info->viewportRect, info->renderHints);
    }
}

