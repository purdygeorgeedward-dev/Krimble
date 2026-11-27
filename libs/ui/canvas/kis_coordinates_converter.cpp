/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <cmath>

#include "kis_coordinates_converter.h"
#include "KoViewTransformStillPoint.h"

#include <QtMath>
#include <QTransform>
#include <KoViewConverter.h>

#include <kis_config.h>
#include <kis_image.h>
#include <kis_algebra_2d.h>
#include <kis_assert.h>
#include <KisValueCache.h>
#include <KisPortingUtils.h>

struct KisCoordinatesConverter::Private {
    Private():
        imageXRes(1.0),
        imageYRes(1.0),
        isXAxisMirrored(false),
        isYAxisMirrored(false),
        isRotating(false),
        isNativeGesture(false),
        rotationAngle(0.0),
        rotationBaseAngle(0.0),
        rotationIsOrthogonal(true),
        devicePixelRatio(1.0),
        standardZoomLevels(this)
    {
    }

    QRect imageBounds;
    QRect extraReferencesBounds;
    qreal imageXRes;
    qreal imageYRes;

    bool isXAxisMirrored;
    bool isYAxisMirrored;
    bool isRotating;
    bool isNativeGesture;
    qreal rotationAngle;
    qreal rotationBaseAngle;
    bool rotationIsOrthogonal;
    QSizeF canvasWidgetSize;
    qreal devicePixelRatio;
    QPointF documentOffset;
    QPointF preferredTransformationCenterImage;
    QPoint minimumOffset;
    QPoint maximumOffset;

    qreal minZoom {0.01};
    qreal maxZoom {9.0};

    struct StandardZoomLevelsInitializer {
        StandardZoomLevelsInitializer(Private *d) : m_d(d) {}
        QVector<qreal> initialize() const;
        Private *m_d;
    };

    KisValueCache<StandardZoomLevelsInitializer> standardZoomLevels;

    QTransform flakeToWidget;
    QTransform rotationBaseTransform;
    QTransform imageToDocument;
    QTransform documentToFlake;
    QTransform widgetToViewport;

    QPointF preferredTransformationCenterInDocumentPixels() const {
        return imageToDocument.map(preferredTransformationCenterImage);
    }
};

QPointF KisCoordinatesConverter::centeringCorrection() const
{
    KisConfig cfg(true);

    QSize documentSize = imageRectInWidgetPixels().toAlignedRect().size();
    QPointF dPoint(documentSize.width(), documentSize.height());
    QPointF wPoint(m_d->canvasWidgetSize.width(), m_d->canvasWidgetSize.height());

    QPointF minOffset = -cfg.vastScrolling() * wPoint;
    QPointF maxOffset = dPoint - wPoint + cfg.vastScrolling() * wPoint;

    QPointF range = maxOffset - minOffset;

    range.rx() = qMin(range.x(), (qreal)0.0);
    range.ry() = qMin(range.y(), (qreal)0.0);

    range /= 2;

    return -range;
}

void KisCoordinatesConverter::recalculateOffsetBoundsAndCrop()
{
    if (!m_d->canvasWidgetSize.isValid()) return;

    KisConfig cfg(true);

    const QRect refRect = imageToWidget(m_d->extraReferencesBounds).toAlignedRect();

    QRect documentRect = imageRectInWidgetPixels().toAlignedRect();
    QPointF dPointMax(qMax(documentRect.width(), refRect.right() + 1 - documentRect.x()),
                      qMax(documentRect.height(),  refRect.bottom() + 1 - documentRect.y()));
    QPointF dPointMin(qMin(0, refRect.left() - documentRect.x()),
                      qMin(0,  refRect.top() - documentRect.y()));
    QPointF wPoint(m_d->canvasWidgetSize.width(), m_d->canvasWidgetSize.height());

    QPointF minOffset = dPointMin - cfg.vastScrolling() * wPoint;
    QPointF maxOffset = dPointMax - wPoint + cfg.vastScrolling() * wPoint;

    m_d->minimumOffset = minOffset.toPoint();
    m_d->maximumOffset = maxOffset.toPoint();

    const QRectF limitRect(m_d->minimumOffset, m_d->maximumOffset);

    if (!limitRect.contains(m_d->documentOffset)) {
        m_d->documentOffset = snapToDevicePixel(KisAlgebra2D::clampPoint(m_d->documentOffset, limitRect));
        qDebug() << "    corrected offset:" << m_d->documentOffset;
        correctTransformationToOffset();
    }
}

QPoint KisCoordinatesConverter::minimumOffset() const
{
    return m_d->minimumOffset;
}

QPoint KisCoordinatesConverter::maximumOffset() const
{
    return m_d->maximumOffset;
}

void KisCoordinatesConverter::correctOffsetToTransformationAndSnap()
{
    m_d->documentOffset = snapToDevicePixel(-(imageRectInWidgetPixels().topLeft() -
          centeringCorrection()));
}

void KisCoordinatesConverter::correctTransformationToOffset()
{
    QPointF topLeft = imageRectInWidgetPixels().topLeft();
    QPointF diff = (-topLeft) - m_d->documentOffset;
    diff += centeringCorrection();
    m_d->flakeToWidget *= QTransform::fromTranslate(diff.x(), diff.y());
}

void KisCoordinatesConverter::resetPreferredTransformationCenter()
{
    m_d->preferredTransformationCenterImage = widgetToImage(this->widgetCenterPoint());
}

void KisCoordinatesConverter::recalculateTransformations()
{
    m_d->imageToDocument = QTransform::fromScale(1 / m_d->imageXRes,
                                                 1 / m_d->imageYRes);

    qreal zoomX, zoomY;
    KoZoomHandler::zoom(&zoomX, &zoomY);
    m_d->documentToFlake = QTransform::fromScale(zoomX, zoomY);

    correctTransformationToOffset();
    recalculateOffsetBoundsAndCrop();

    QRectF irect = imageRectInWidgetPixels();
    QRectF wrect = QRectF(QPoint(0,0), m_d->canvasWidgetSize);
    QRectF rrect = irect & wrect;

    QTransform reversedTransform = flakeToWidgetTransform().inverted();
    QRectF     canvasBounds      = reversedTransform.mapRect(rrect);
    QPointF    offset            = canvasBounds.topLeft();

    m_d->widgetToViewport = reversedTransform * QTransform::fromTranslate(-offset.x(), -offset.y());
}

KisCoordinatesConverter::KisCoordinatesConverter()
    : m_d(new Private) { }

KisCoordinatesConverter::~KisCoordinatesConverter()
{
    delete m_d;
}

QSizeF KisCoordinatesConverter::getCanvasWidgetSize() const
{
    return m_d->canvasWidgetSize;
}

void KisCoordinatesConverter::setCanvasWidgetSize(QSizeF size)
{
    m_d->canvasWidgetSize = snapWidgetSizeToDevicePixel(size);
    recalculateTransformations();

    resetPreferredTransformationCenter();
}

void KisCoordinatesConverter::setDevicePixelRatio(qreal value)
{
    m_d->devicePixelRatio = value;
}

void KisCoordinatesConverter::setImage(KisImageWSP image)
{
    m_d->imageXRes = image->xRes();
    m_d->imageYRes = image->yRes();

    m_d->imageBounds = image->bounds();
    recalculateZoomLevelLimits();
    recalculateTransformations();

    if (m_d->canvasWidgetSize.isEmpty()) {
        KoZoomHandler::setZoomMode(KoZoomMode::ZOOM_PAGE);
        m_d->preferredTransformationCenterImage = m_d->imageBounds.center();
    } else {
        setZoom(KoZoomMode::ZOOM_PAGE, 777.7, resolutionX(), resolutionY(), std::nullopt);
    }
}

void KisCoordinatesConverter::setExtraReferencesBounds(const QRect &imageRect)
{
    if (imageRect == m_d->extraReferencesBounds) return;

    m_d->extraReferencesBounds = imageRect;
    recalculateTransformations();
}

void KisCoordinatesConverter::setImageBounds(const QRect &rect, const QPointF oldImageStillPoint, const QPointF newImageStillPoint)
{
    if (rect == m_d->imageBounds) return;

    const QPointF oldWidgetStillPoint = imageToWidget(oldImageStillPoint);

    setZoomMode(KoZoomMode::ZOOM_CONSTANT);

    m_d->imageBounds = rect;
    recalculateZoomLevelLimits();
    recalculateTransformations();

    const QPointF newWidgetStillPoint = imageToWidget(newImageStillPoint);
    m_d->documentOffset = snapToDevicePixel(m_d->documentOffset + newWidgetStillPoint - oldWidgetStillPoint);
    recalculateTransformations();

    resetPreferredTransformationCenter();
}

void KisCoordinatesConverter::setImageResolution(qreal xRes, qreal yRes)
{

    if (qFuzzyCompare(xRes, m_d->imageXRes) && qFuzzyCompare(yRes, m_d->imageYRes)) return;

    const QPointF oldImageCenter = imageCenterInWidgetPixel();

    setZoomMode(KoZoomMode::ZOOM_CONSTANT);

    m_d->imageXRes = xRes;
    m_d->imageYRes = yRes;
    recalculateZoomLevelLimits();
    recalculateTransformations();

    const QPointF newImageCenter = imageCenterInWidgetPixel();
    m_d->documentOffset = snapToDevicePixel(m_d->documentOffset + newImageCenter - oldImageCenter);
    recalculateTransformations();

    resetPreferredTransformationCenter();
}

void KisCoordinatesConverter::setDocumentOffset(const QPointF& offset)
{
    setZoomMode(KoZoomMode::ZOOM_CONSTANT);

    m_d->documentOffset = snapToDevicePixel(offset);
    recalculateTransformations();

    resetPreferredTransformationCenter();
}

qreal KisCoordinatesConverter::devicePixelRatio() const
{
    return m_d->devicePixelRatio;
}

QPoint KisCoordinatesConverter::documentOffset() const
{
    return QPoint(int(m_d->documentOffset.x()), int(m_d->documentOffset.y()));
}

QPointF KisCoordinatesConverter::documentOffsetF() const
{
    return m_d->documentOffset;
}

QPointF KisCoordinatesConverter::preferredTransformationCenter() const
{
    return m_d->preferredTransformationCenterImage;
}

qreal KisCoordinatesConverter::rotationAngle() const
{
    return m_d->rotationAngle;
}

void KisCoordinatesConverter::setZoom(qreal zoom)
{
    setZoomMode(KoZoomMode::ZOOM_CONSTANT);

    KoZoomHandler::setZoom(zoom);
    recalculateTransformations();
    resetPreferredTransformationCenter();
}

void KisCoordinatesConverter::setCanvasWidgetSizeKeepZoom(const QSizeF &size)
{
    setCanvasWidgetSize(size);

    if (zoomMode() == KoZoomMode::ZOOM_CONSTANT) {
    } else {
        setZoom(zoomMode(), 777.0, resolutionX(), resolutionY(), std::nullopt);
    }
}

QSizeF KisCoordinatesConverter::snapWidgetSizeToDevicePixel(const QSizeF &size) const
{
    if (qFuzzyCompare(m_d->devicePixelRatio, 1.0)) return size;

    const int viewportWidth = static_cast<int>(size.width() * m_d->devicePixelRatio);
    const int viewportHeight = static_cast<int>(size.height() * m_d->devicePixelRatio);

    return QSizeF(viewportWidth, viewportHeight) / m_d->devicePixelRatio;
}

QSize KisCoordinatesConverter::viewportDevicePixelSize() const
{
    return qFuzzyCompare(m_d->devicePixelRatio, 1.0) ?
        m_d->canvasWidgetSize.toSize() :
        (m_d->canvasWidgetSize * m_d->devicePixelRatio).toSize();
}

void KisCoordinatesConverter::setZoom(KoZoomMode::Mode mode, qreal zoom, qreal resolutionX, qreal resolutionY, const std::optional<KoViewTransformStillPoint> &stillPoint)
{
    const int cfgMargin = zoomMarginSize();

    auto updateDisplayResolution = [&]() {
        if (!qFuzzyCompare(resolutionX, this->resolutionX()) || !qFuzzyCompare(resolutionY, this->resolutionY())) {
            setResolution(resolutionX, resolutionY);
            recalculateZoomLevelLimits();
        }
    };

    if(mode == KoZoomMode::ZOOM_CONSTANT) {
        if(qFuzzyIsNull(zoom)) return;

        zoom = clampZoom(zoom);

        KoViewTransformStillPoint effectiveStillPoint =
            stillPoint ? *stillPoint :
            KoViewTransformStillPoint(m_d->preferredTransformationCenterInDocumentPixels(), widgetCenterPoint());

        updateDisplayResolution();
        KoZoomHandler::setZoom(zoom);
        KoZoomHandler::setZoomMode(mode);
        recalculateTransformations();

        const QPointF newStillPoint = documentToWidget(effectiveStillPoint.docPoint());
        const QPointF offset = newStillPoint - effectiveStillPoint.viewPoint();
        m_d->documentOffset = snapToDevicePixel(m_d->documentOffset + offset);
        recalculateTransformations();

        if (stillPoint) {
            resetPreferredTransformationCenter();
        }

    } else if (mode == KoZoomMode::ZOOM_PAGE || mode == KoZoomMode::ZOOM_WIDTH || mode == KoZoomMode::ZOOM_HEIGHT) {
        updateDisplayResolution();
        recalculateTransformations();

        KIS_SAFE_ASSERT_RECOVER_RETURN(!m_d->canvasWidgetSize.isEmpty());

        const QSizeF documentSize = imageRectInWidgetPixels().size();
        const qreal zoomCoeffX = (m_d->canvasWidgetSize.width() - 2 * cfgMargin) / documentSize.width();
        const qreal zoomCoeffY = (m_d->canvasWidgetSize.height() - 2 * cfgMargin) / documentSize.height();

        const bool fitToWidth = [&]() {
            if (mode == KoZoomMode::ZOOM_PAGE) {
                return zoomCoeffX < zoomCoeffY;
            } else if (mode == KoZoomMode::ZOOM_HEIGHT) {
                return false;
            } else if (mode == KoZoomMode::ZOOM_WIDTH) {
                return true;
            }
            Q_UNREACHABLE_RETURN(true);
        }();

        KoZoomHandler::setZoom(this->zoom() * (fitToWidth ? zoomCoeffX : zoomCoeffY));
        KoZoomHandler::setZoomMode(mode);
        recalculateTransformations();

        const QPointF offset = imageCenterInWidgetPixel() - widgetCenterPoint();

        QPointF newDocumentOffset = m_d->documentOffset + offset;

        if (fitToWidth) {
            newDocumentOffset.setX(-cfgMargin);
        } else {
            newDocumentOffset.setY(-cfgMargin);
        }

        m_d->documentOffset = snapToDevicePixel(newDocumentOffset);
        recalculateTransformations();

        resetPreferredTransformationCenter();
    }
}

void KisCoordinatesConverter::zoomTo(const QRectF &zoomRectWidget)
{
    KIS_SAFE_ASSERT_RECOVER_RETURN(!zoomRectWidget.isEmpty());

    const QPointF zoomPortionCenterInImagePixels = widgetToImage(zoomRectWidget.center());

    const qreal zoomCoeffX = m_d->canvasWidgetSize.width() / zoomRectWidget.width();
    const qreal zoomCoeffY = m_d->canvasWidgetSize.height() / zoomRectWidget.height();

    const bool fitToWidth = zoomCoeffX < zoomCoeffY;

    KoZoomHandler::setZoom(this->zoom() * (fitToWidth ? zoomCoeffX : zoomCoeffY));
    KoZoomHandler::setZoomMode(KoZoomMode::ZOOM_CONSTANT);
    recalculateTransformations();

    const QPointF offset = imageToWidget(zoomPortionCenterInImagePixels) - widgetCenterPoint();
    QPointF newDocumentOffset = m_d->documentOffset + offset;

    m_d->documentOffset = snapToDevicePixel(newDocumentOffset);
    recalculateTransformations();

    resetPreferredTransformationCenter();
}

qreal KisCoordinatesConverter::effectiveZoom() const
{
    qreal scaleX, scaleY;
    this->imageScale(&scaleX, &scaleY);

    if (scaleX != scaleY) {
        qWarning() << "WARNING: Zoom is not isotropic!"  << ppVar(scaleX) << ppVar(scaleY) << ppVar(qFuzzyCompare(scaleX, scaleY));
    }

    return 0.5 * (scaleX + scaleY);
}

qreal KisCoordinatesConverter::effectivePhysicalZoom() const
{
    qreal scaleX, scaleY;
    this->imagePhysicalScale(&scaleX, &scaleY);

    if (scaleX != scaleY) {
        qWarning() << "WARNING: Zoom is not isotropic!"  << ppVar(scaleX) << ppVar(scaleY) << ppVar(qFuzzyCompare(scaleX, scaleY));
    }

    return 0.5 * (scaleX + scaleY);
}

void KisCoordinatesConverter::enableNatureGestureFlag()
{
    m_d->isNativeGesture = true;
}

void KisCoordinatesConverter::beginRotation()
{
    KIS_SAFE_ASSERT_RECOVER_NOOP(!m_d->isRotating);

    m_d->rotationBaseTransform = m_d->flakeToWidget;
    m_d->rotationBaseAngle = m_d->rotationAngle;
    m_d->isRotating = true;
}

void KisCoordinatesConverter::endRotation()
{
    KIS_SAFE_ASSERT_RECOVER_NOOP(m_d->isRotating);

    m_d->isNativeGesture = false;
    m_d->isRotating = false;
}

void KisCoordinatesConverter::rotate(const std::optional<KoViewTransformStillPoint> &stillPoint, qreal angle)
{
    setZoomMode(KoZoomMode::ZOOM_CONSTANT);

    KoViewTransformStillPoint effectiveStillPoint =
        stillPoint ? *stillPoint :
        KoViewTransformStillPoint(m_d->preferredTransformationCenterInDocumentPixels(), widgetCenterPoint());

    QTransform rot;
    rot.rotate(angle);

    if (!m_d->isNativeGesture && m_d->isRotating)
    {
        m_d->flakeToWidget = m_d->rotationBaseTransform;
        m_d->rotationAngle = std::fmod(m_d->rotationBaseAngle + angle, 360.0);
    }
    else
    {
        m_d->rotationAngle = std::fmod(m_d->rotationAngle + angle, 360.0);
    }

    {
        const qreal numQuadrants = m_d->rotationAngle / 90.0;
        m_d->rotationIsOrthogonal = std::floor(numQuadrants) == numQuadrants;
    }

    m_d->flakeToWidget *= rot;
    correctOffsetToTransformationAndSnap();
    recalculateTransformations();

    const QPointF newStillPoint = documentToWidget(effectiveStillPoint.docPoint());
    const QPointF offset = newStillPoint - effectiveStillPoint.viewPoint();
    m_d->documentOffset = snapToDevicePixel(m_d->documentOffset + offset);
    recalculateTransformations();

    if (stillPoint) {
        resetPreferredTransformationCenter();
    }
}

void KisCoordinatesConverter::mirror(const std::optional<KoViewTransformStillPoint> &stillPoint, bool mirrorXAxis, bool mirrorYAxis)
{
    bool keepOrientation = false;

    KoViewTransformStillPoint effectiveStillPoint =
        stillPoint ? *stillPoint :
        KoViewTransformStillPoint(m_d->preferredTransformationCenterInDocumentPixels(), widgetCenterPoint());

    if (kisSquareDistance(effectiveStillPoint.viewPoint(), widgetCenterPoint()) > 2.0) {
        setZoomMode(KoZoomMode::ZOOM_CONSTANT);
    }

    const QPointF oldDocumentOffset = m_d->documentOffset;

    bool       doXMirroring = m_d->isXAxisMirrored ^ mirrorXAxis;
    bool       doYMirroring = m_d->isYAxisMirrored ^ mirrorYAxis;
    qreal      scaleX       = doXMirroring ? -1.0 : 1.0;
    qreal      scaleY       = doYMirroring ? -1.0 : 1.0;
    QTransform mirror       = QTransform::fromScale(scaleX, scaleY);

    QTransform rot;
    rot.rotate(m_d->rotationAngle);

    m_d->flakeToWidget *= QTransform::fromTranslate(-effectiveStillPoint.viewPoint().x(),-effectiveStillPoint.viewPoint().y());

    if (keepOrientation) {
        m_d->flakeToWidget *= rot.inverted();
    }

    m_d->flakeToWidget *= mirror;

    if (keepOrientation) {
        m_d->flakeToWidget *= rot;
    }

    m_d->flakeToWidget *= QTransform::fromTranslate(effectiveStillPoint.viewPoint().x(),effectiveStillPoint.viewPoint().y());

    if (!keepOrientation && (doXMirroring ^ doYMirroring)) {
        m_d->rotationAngle = -m_d->rotationAngle;
    }

    m_d->isXAxisMirrored = mirrorXAxis;
    m_d->isYAxisMirrored = mirrorYAxis;

    correctOffsetToTransformationAndSnap();

    if (zoomMode() != KoZoomMode::ZOOM_CONSTANT) {
        m_d->documentOffset = oldDocumentOffset;
    } else {
        recalculateTransformations();
        const QPointF newStillPoint = documentToWidget(effectiveStillPoint.docPoint());
        const QPointF offset = newStillPoint - effectiveStillPoint.viewPoint();
        m_d->documentOffset = snapToDevicePixel(m_d->documentOffset + offset);
    }

    recalculateTransformations();

    if (stillPoint) {
        resetPreferredTransformationCenter();
    }
}

bool KisCoordinatesConverter::xAxisMirrored() const
{
    return m_d->isXAxisMirrored;
}

bool KisCoordinatesConverter::yAxisMirrored() const
{
    return m_d->isYAxisMirrored;
}

void KisCoordinatesConverter::resetRotation(const std::optional<KoViewTransformStillPoint> &stillPoint)
{
    KoViewTransformStillPoint effectiveStillPoint =
        stillPoint ? *stillPoint :
        KoViewTransformStillPoint(m_d->preferredTransformationCenterInDocumentPixels(), widgetCenterPoint());

    QTransform rot;
    rot.rotate(-m_d->rotationAngle);

    m_d->flakeToWidget *= rot;
    m_d->rotationAngle = 0.0;
    m_d->rotationIsOrthogonal = true;

    correctOffsetToTransformationAndSnap();
    recalculateTransformations();

    const QPointF newStillPoint = documentToWidget(effectiveStillPoint.docPoint());
    const QPointF offset = newStillPoint - effectiveStillPoint.viewPoint();
    m_d->documentOffset = snapToDevicePixel(m_d->documentOffset + offset);
    recalculateTransformations();

    if (stillPoint) {
        resetPreferredTransformationCenter();
    }
}

QTransform KisCoordinatesConverter::imageToWidgetTransform() const {
    return m_d->imageToDocument * m_d->documentToFlake * m_d->flakeToWidget;
}

QTransform KisCoordinatesConverter::imageToDocumentTransform() const {
    return m_d->imageToDocument;
}

QTransform KisCoordinatesConverter::documentToFlakeTransform() const {
    return m_d->documentToFlake;
}

QTransform KisCoordinatesConverter::flakeToWidgetTransform() const {
    return m_d->flakeToWidget;
}

QTransform KisCoordinatesConverter::documentToWidgetTransform() const {
    return m_d->documentToFlake * m_d->flakeToWidget;
}

QTransform KisCoordinatesConverter::viewportToWidgetTransform() const {
    return m_d->widgetToViewport.inverted();
}

QTransform KisCoordinatesConverter::imageToViewportTransform() const {
    return m_d->imageToDocument * m_d->documentToFlake * m_d->flakeToWidget * m_d->widgetToViewport;
}

void KisCoordinatesConverter::getQPainterCheckersInfo(QTransform *transform,
                                                      QPointF *brushOrigin,
                                                      QPolygonF *polygon,
                                                      const bool scrollCheckers) const
{

    QRectF imageRect = imageRectInViewportPixels();
    imageRect.adjust(0,0,-0.5,-0.5);

    if (scrollCheckers) {
        *transform = viewportToWidgetTransform();
        *polygon = imageRect;
        *brushOrigin = imageToViewport(QPointF(0,0));
    }
    else {
        *transform = QTransform();
        *polygon = viewportToWidgetTransform().map(imageRect);
        *brushOrigin = QPoint(0,0);
    }
}

void KisCoordinatesConverter::getOpenGLCheckersInfo(const QRectF &viewportRect,
                                                    QTransform *textureTransform,
                                                    QTransform *modelTransform,
                                                    QRectF *textureRect,
                                                    QRectF *modelRect,
                                                    const bool scrollCheckers) const
{
    if(scrollCheckers) {
        *textureTransform = QTransform();
        *textureRect = QRectF(0, 0, viewportRect.width(),viewportRect.height());
    }
    else {
        *textureTransform = viewportToWidgetTransform();
        *textureRect = viewportRect;
    }

    *modelTransform = viewportToWidgetTransform();
    *modelRect = viewportRect;
}

QPointF KisCoordinatesConverter::imageCenterInWidgetPixel() const
{
    QPolygonF poly = imageToWidget(QPolygon(m_d->imageBounds));
    return (poly[0] + poly[1] + poly[2] + poly[3]) / 4.0;
}

QRectF KisCoordinatesConverter::imageRectInWidgetPixels() const
{
    return imageToWidget(m_d->imageBounds);
}

QRectF KisCoordinatesConverter::imageRectInViewportPixels() const
{
    return imageToViewport(m_d->imageBounds);
}

QRect KisCoordinatesConverter::imageRectInImagePixels() const
{
    return m_d->imageBounds;
}

QRectF KisCoordinatesConverter::imageRectInDocumentPixels() const
{
    return imageToDocument(m_d->imageBounds);
}

QSizeF KisCoordinatesConverter::imageSizeInFlakePixels() const
{
    qreal scaleX, scaleY;
    imageScale(&scaleX, &scaleY);
    QSize imageSize = m_d->imageBounds.size();

    return QSizeF(imageSize.width() * scaleX, imageSize.height() * scaleY);
}

QRectF KisCoordinatesConverter::widgetRectInFlakePixels() const
{
    return widgetToFlake(QRectF(QPoint(0,0), m_d->canvasWidgetSize));
}

QRectF KisCoordinatesConverter::widgetRectInImagePixels() const
{
    return widgetToImage(QRectF(QPoint(0,0), m_d->canvasWidgetSize));
}

QPointF KisCoordinatesConverter::flakeCenterPoint() const
{
    QRectF widgetRect = widgetRectInFlakePixels();
    return QPointF(widgetRect.left() + widgetRect.width() / 2,
                   widgetRect.top() + widgetRect.height() / 2);
}

QPointF KisCoordinatesConverter::widgetCenterPoint() const
{
    return QPointF(m_d->canvasWidgetSize.width() / 2.0, m_d->canvasWidgetSize.height() / 2.0);
}

void KisCoordinatesConverter::imageScale(qreal *scaleX, qreal *scaleY) const
{
    qreal zoomX, zoomY;
    KoZoomHandler::zoom(&zoomX, &zoomY);

    qreal resX = m_d->imageXRes;
    qreal resY = m_d->imageYRes;

    *scaleX = zoomX / resX;
    *scaleY = zoomY / resY;
}

void KisCoordinatesConverter::imagePhysicalScale(qreal *scaleX, qreal *scaleY) const
{
    imageScale(scaleX, scaleY);
    *scaleX *= m_d->devicePixelRatio;
    *scaleY *= m_d->devicePixelRatio;
}

QPointF KisCoordinatesConverter::snapToDevicePixel(const QPointF &point) const
{
    if (!m_d->rotationIsOrthogonal) {
        return point;
    }

    QPoint devicePixel = (point * m_d->devicePixelRatio).toPoint();
    return QPointF(devicePixel) / m_d->devicePixelRatio;
}

QTransform KisCoordinatesConverter::viewToWidget() const
{
    return flakeToWidgetTransform();
}

QTransform KisCoordinatesConverter::widgetToView() const
{
    return flakeToWidgetTransform().inverted();
}

qreal KisCoordinatesConverter::minZoom() const
{
    return m_d->minZoom;
}
qreal KisCoordinatesConverter::maxZoom() const
{
    return m_d->maxZoom;
}
qreal KisCoordinatesConverter::clampZoom(qreal zoom) const
{
    return std::clamp(zoom, minZoom(), maxZoom());
}

QVector<qreal> KisCoordinatesConverter::standardZoomLevels() const
{
    return m_d->standardZoomLevels.value();
}

QVector<qreal> KisCoordinatesConverter::Private::StandardZoomLevelsInitializer::initialize() const
{
    return KoZoomMode::generateStandardZoomLevels(m_d->minZoom, m_d->maxZoom);
}

void KisCoordinatesConverter::recalculateZoomLevelLimits()
{
    qreal minDimension = 0.0;

    if (m_d->imageBounds.width() < m_d->imageBounds.height()) {
        minDimension = m_d->imageBounds.width() * resolutionX() / m_d->imageXRes;
    } else {
        minDimension = m_d->imageBounds.height() * resolutionY() / m_d->imageYRes;
    }

    m_d->minZoom = qMin(100.0 / minDimension, 0.1);
    m_d->maxZoom = 90.0;
    m_d->standardZoomLevels.clear();
}

qreal KisCoordinatesConverter::findNextZoom(qreal currentZoom, const QVector<qreal> &zoomLevels)
{
    return KoZoomMode::findNextZoom(currentZoom, zoomLevels);
}

qreal KisCoordinatesConverter::findPrevZoom(qreal currentZoom, const QVector<qreal> &zoomLevels)
{
    return KoZoomMode::findPrevZoom(currentZoom, zoomLevels);
}

KoViewTransformStillPoint KisCoordinatesConverter::makeWidgetStillPoint(const QPointF &viewPoint) const
{
    return {widgetToDocument(viewPoint), viewPoint};
}

KoViewTransformStillPoint KisCoordinatesConverter::makeDocStillPoint(const QPointF &docPoint) const
{
    return {docPoint, documentToWidget(docPoint)};
}