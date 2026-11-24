/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <cmath>
#include <QtMath>
#include "kis_free_transform_strategy.h"

#include <QPointF>
#include <QPainter>
#include <QPainterPath>
#include <QMatrix4x4>

#include <KoResourcePaths.h>

#include "KoColorDisplayRendererInterface.h"
#include "kis_coordinates_converter.h"
#include "tool_transform_args.h"
#include "transform_transaction_properties.h"
#include "krita_utils.h"
#include "kis_cursor.h"
#include "kis_transform_utils.h"
#include "kis_free_transform_strategy_gsl_helpers.h"
#include "kis_algebra_2d.h"

namespace {
enum StrokeFunction {
    ROTATE = 0,
    MOVE,
    RIGHTSCALE,
    TOPRIGHTSCALE,
    TOPSCALE,
    TOPLEFTSCALE,
    LEFTSCALE,
    BOTTOMLEFTSCALE,
    BOTTOMSCALE,
    BOTTOMRIGHTSCALE,
    BOTTOMSHEAR,
    RIGHTSHEAR,
    TOPSHEAR,
    LEFTSHEAR,
    MOVECENTER,
    PERSPECTIVE,
    CAMERAHEIGHT,
    ROTATEBOUNDS,
    KRIMBLENONE
};
}

struct KisFreeTransformStrategy::Private
{
    Private(KisFreeTransformStrategy *_q,
            const KisCoordinatesConverter *_converter,
            ToolTransformArgs &_currentArgs,
            TransformTransactionProperties &_transaction)
        : q(_q),
          converter(_converter),
          currentArgs(_currentArgs),
          transaction(_transaction),
          imageTooBig(false),
          isTransforming(false)
    {
        scaleCursors[0] = KisCursor::sizeHorCursor();
        scaleCursors[1] = KisCursor::sizeFDiagCursor();
        scaleCursors[2] = KisCursor::sizeVerCursor();
        scaleCursors[3] = KisCursor::sizeBDiagCursor();
        scaleCursors[4] = KisCursor::sizeHorCursor();
        scaleCursors[5] = KisCursor::sizeFDiagCursor();
        scaleCursors[6] = KisCursor::sizeVerCursor();
        scaleCursors[7] = KisCursor::sizeBDiagCursor();

        shearCursorPixmap = QIcon(":/shear_cursor.svg").pixmap(22, 22);
        rotateHandlesCursor = QCursor(QIcon(":/rotate_cursor_handles.svg").pixmap(32, 32));
    }

    KisFreeTransformStrategy *q;

    const KisCoordinatesConverter *converter;

    ToolTransformArgs &currentArgs;
    TransformTransactionProperties &transaction;

    QTransform thumbToImageTransform;
    QImage originalImage;

    QTransform paintingTransform;
    QPointF paintingOffset;

    QTransform handlesTransform;

    StrokeFunction function {MOVE};

    struct HandlePoints {
        QPointF topLeft;
        QPointF topMiddle;
        QPointF topRight;

        QPointF middleLeft;
        QPointF rotationCenter;
        QPointF middleRight;

        QPointF bottomLeft;
        QPointF bottomMiddle;
        QPointF bottomRight;
    };
    HandlePoints transformedHandles;

    QRectF bounds;
    QTransform boundsTransform;

    QTransform transform;

    QCursor scaleCursors[8];
    QPixmap shearCursorPixmap;
    QCursor rotateHandlesCursor;

    bool imageTooBig {false};

    ToolTransformArgs clickArgs;
    QPointF clickPos;
    QTransform clickTransform;

    bool isTransforming {false};

    QCursor getScaleCursor(const QPointF &handlePt);
    QCursor getShearCursor(const QPointF &start, const QPointF &end);
    void recalculateTransformations();
    void recalculateTransformedHandles();
    void recalculateBounds();
};

KisFreeTransformStrategy::KisFreeTransformStrategy(const KisCoordinatesConverter *converter,
                                                   KoSnapGuide *snapGuide,
                                                   ToolTransformArgs &currentArgs,
                                                   TransformTransactionProperties &transaction)
    : KisSimplifiedActionPolicyStrategy(converter, snapGuide),
      m_d(new Private(this, converter, currentArgs, transaction))
{
}

KisFreeTransformStrategy::~KisFreeTransformStrategy()
{
}

namespace {
QPointF middleLeft(const QRectF &rect)
{
    return (rect.topLeft() + rect.bottomLeft()) * 0.5;
}
QPointF middleRight(const QRectF &rect)
{
    return (rect.topRight() + rect.bottomRight()) * 0.5;
}
QPointF bottomMiddle(const QRectF &rect)
{
    return (rect.bottomLeft() + rect.bottomRight()) * 0.5;
}
QPointF topMiddle(const QRectF &rect)
{
    return (rect.topLeft() + rect.topRight()) * 0.5;
}
}

void KisFreeTransformStrategy::Private::recalculateBounds()
{
    const QPolygonF &convexHull = transaction.convexHull();
    if (!convexHull.isEmpty()) {
        bounds = boundsTransform.inverted().map(convexHull).boundingRect();
    } else {
        bounds = boundsTransform.inverted().mapRect(transaction.originalRect());
    }
}

void KisFreeTransformStrategy::Private::recalculateTransformedHandles()
{
    QTransform boundsFullTransform = boundsTransform * transform;
    transformedHandles.topLeft = boundsFullTransform.map(bounds.topLeft());
    transformedHandles.topMiddle = boundsFullTransform.map(topMiddle(bounds));
    transformedHandles.topRight = boundsFullTransform.map(bounds.topRight());

    transformedHandles.middleLeft = boundsFullTransform.map(middleLeft(bounds));
    transformedHandles.middleRight = boundsFullTransform.map(middleRight(bounds));

    transformedHandles.bottomLeft = boundsFullTransform.map(bounds.bottomLeft());
    transformedHandles.bottomMiddle = boundsFullTransform.map(bottomMiddle(bounds));
    transformedHandles.bottomRight = boundsFullTransform.map(bounds.bottomRight());

    transformedHandles.rotationCenter = transform.map(currentArgs.originalCenter() + currentArgs.rotationCenterOffset());
}

void KisFreeTransformStrategy::setTransformFunction(const QPointF &mousePos, bool perspectiveModifierActive, bool shiftModifierActive, bool altModifierActive)
{
    Q_UNUSED(shiftModifierActive);

    Q_UNUSED(perspectiveModifierActive);

    QTransform boundsFullTransform = m_d->boundsTransform * m_d->transform;
    QPolygonF transformedPolygon = boundsFullTransform.map(QPolygonF(m_d->bounds));
    qreal handleRadius = KisTransformUtils::effectiveHandleGrabRadius(m_d->converter);
    qreal rotationHandleRadius = KisTransformUtils::effectiveHandleGrabRadius(m_d->converter);

       StrokeFunction defaultFunction = MOVE;
       if (transformedPolygon.containsPoint(mousePos, Qt::OddEvenFill))
          defaultFunction = MOVE;
       else
          defaultFunction = MOVE;

    {
        const ToolTransformArgs::FreeGesture gesture = m_d->currentArgs.freeGesture();
        const bool insideBox = transformedPolygon.containsPoint(mousePos, Qt::OddEvenFill);
        if (gesture == ToolTransformArgs::GestureMove) {
            m_d->function = MOVE;
            return;
        } else if (gesture == ToolTransformArgs::GestureRotate) {
            m_d->function = insideBox ? MOVE : ROTATE;
            return;
        } else if (gesture == ToolTransformArgs::GestureShear) {
            KisTransformUtils::HandleChooser<StrokeFunction> shearChooser(mousePos, MOVE);
            shearChooser.addFunction(m_d->transformedHandles.topMiddle, handleRadius, TOPSHEAR);
            shearChooser.addFunction(m_d->transformedHandles.bottomMiddle, handleRadius, BOTTOMSHEAR);
            shearChooser.addFunction(m_d->transformedHandles.middleRight, handleRadius, RIGHTSHEAR);
            shearChooser.addFunction(m_d->transformedHandles.middleLeft, handleRadius, LEFTSHEAR);
            m_d->function = shearChooser.function();
            return;
        }
    }

    KisTransformUtils::HandleChooser<StrokeFunction>
        handleChooser(mousePos, defaultFunction);

    handleChooser.addFunction(m_d->transformedHandles.topMiddle,
                              handleRadius, TOPSCALE);
    handleChooser.addFunction(m_d->transformedHandles.topRight,
                              handleRadius, TOPRIGHTSCALE);
    handleChooser.addFunction(m_d->transformedHandles.middleRight,
                              handleRadius, RIGHTSCALE);

    handleChooser.addFunction(m_d->transformedHandles.bottomRight,
                              handleRadius, BOTTOMRIGHTSCALE);
    handleChooser.addFunction(m_d->transformedHandles.bottomMiddle,
                              handleRadius, BOTTOMSCALE);
    handleChooser.addFunction(m_d->transformedHandles.bottomLeft,
                              handleRadius, BOTTOMLEFTSCALE);
    handleChooser.addFunction(m_d->transformedHandles.middleLeft,
                              handleRadius, LEFTSCALE);
    handleChooser.addFunction(m_d->transformedHandles.topLeft,
                              handleRadius, TOPLEFTSCALE);

    m_d->function = handleChooser.function();

}

bool KisFreeTransformStrategy::shiftModifierIsUsed() const
{
    return true;
}

QCursor KisFreeTransformStrategy::Private::getScaleCursor(const QPointF &handlePt)
{
    QPointF handlePtInWidget = converter->imageToWidget(handlePt);
    QPointF centerPtInWidget = converter->imageToWidget(currentArgs.transformedCenter());

    QPointF direction = handlePtInWidget - centerPtInWidget;
    qreal angle = atan2(direction.y(), direction.x());
    angle = normalizeAngle(angle);

    int octant = qRound(angle * 4. / M_PI) % 8;
    return scaleCursors[octant];
}

QCursor KisFreeTransformStrategy::Private::getShearCursor(const QPointF &start, const QPointF &end)
{
    QPointF startPtInWidget = converter->imageToWidget(start);
    QPointF endPtInWidget = converter->imageToWidget(end);
    QPointF direction = endPtInWidget - startPtInWidget;

    qreal angle = atan2(-direction.y(), direction.x());
    return QCursor(shearCursorPixmap.transformed(QTransform().rotateRadians(-angle)));
}

QCursor KisFreeTransformStrategy::getCurrentCursor() const
{
    QCursor cursor;

    switch (m_d->function) {
    case MOVE:
        cursor = KisCursor::moveCursor();
        break;
    case ROTATEBOUNDS:
        cursor = m_d->rotateHandlesCursor;
        break;
    case KRIMBLENONE:
        break;
    case ROTATE:
        cursor = KisCursor::rotateCursor();
        break;
    case PERSPECTIVE:
        cursor = KisCursor::rotateCursor();
        break;
    case CAMERAHEIGHT:
        cursor = KisCursor::sizeVerCursor();
        break;
    case RIGHTSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.middleRight);
        break;
    case TOPSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.topMiddle);
        break;
    case LEFTSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.middleLeft);
        break;
    case BOTTOMSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.bottomMiddle);
        break;
    case TOPRIGHTSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.topRight);
        break;
    case BOTTOMLEFTSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.bottomLeft);
        break;
    case TOPLEFTSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.topLeft);
        break;
    case BOTTOMRIGHTSCALE:
        cursor = m_d->getScaleCursor(m_d->transformedHandles.bottomRight);
        break;
    case MOVECENTER:
        cursor = KisCursor::handCursor();
        break;
    case BOTTOMSHEAR:
        cursor = m_d->getShearCursor(m_d->transformedHandles.bottomLeft, m_d->transformedHandles.bottomRight);
        break;
    case RIGHTSHEAR:
        cursor = m_d->getShearCursor(m_d->transformedHandles.bottomRight, m_d->transformedHandles.topRight);
        break;
    case TOPSHEAR:
        cursor = m_d->getShearCursor(m_d->transformedHandles.topRight, m_d->transformedHandles.topLeft);
        break;
    case LEFTSHEAR:
        cursor = m_d->getShearCursor(m_d->transformedHandles.topLeft, m_d->transformedHandles.bottomLeft);
        break;
    }

    return cursor;
}

void KisFreeTransformStrategy::paint(QPainter &gc, const KoColorDisplayRendererInterface *displayRendererInterface)
{
    gc.save();

    gc.setOpacity(m_d->transaction.basePreviewOpacity());
    gc.setTransform(m_d->paintingTransform, true);
    gc.drawImage(m_d->paintingOffset, originalImage());

    gc.restore();

    QRectF handleRect =
        KisTransformUtils::handleRect(KisTransformUtils::handleVisualRadius,
                                      m_d->handlesTransform,
                                      m_d->bounds, 0, 0);

    qreal rX = 1;
    qreal rY = 1;
    QRectF rotationCenterRect =
        KisTransformUtils::handleRect(KisTransformUtils::rotationHandleVisualRadius,
                                      m_d->handlesTransform,
                                      m_d->bounds,
                                      &rX,
                                      &rY);

    QPainterPath handles;

    handles.moveTo(m_d->bounds.topLeft());
    handles.lineTo(m_d->bounds.topRight());
    handles.lineTo(m_d->bounds.bottomRight());
    handles.lineTo(m_d->bounds.bottomLeft());
    handles.lineTo(m_d->bounds.topLeft());

    handles.addRect(handleRect.translated(m_d->bounds.topLeft()));
    handles.addRect(handleRect.translated(m_d->bounds.topRight()));
    handles.addRect(handleRect.translated(m_d->bounds.bottomLeft()));
    handles.addRect(handleRect.translated(m_d->bounds.bottomRight()));
    handles.addRect(handleRect.translated(middleLeft(m_d->bounds)));
    handles.addRect(handleRect.translated(middleRight(m_d->bounds)));
    handles.addRect(handleRect.translated(topMiddle(m_d->bounds)));
    handles.addRect(handleRect.translated(bottomMiddle(m_d->bounds)));

    Q_UNUSED(rotationCenterRect);
    Q_UNUSED(rX);
    Q_UNUSED(rY);

    gc.save();

    if (m_d->isTransforming) {
        gc.setOpacity(0.1);
    }

    QPainterPath mappedHandles = m_d->handlesTransform.map(handles);

    QPen pen[2];
    pen[0].setWidth(decorationThickness());
    pen[0].setCosmetic(true);
    pen[1].setWidth(decorationThickness() * 2);
    pen[1].setCosmetic(true);
    pen[1].setColor(displayRendererInterface->convertColorToDisplayColorSpace(KoColor(Qt::lightGray, KoColorSpaceRegistry::instance()->rgb8())));

    {
        const qreal kRotateTargetRadius = KisTransformUtils::handleVisualRadius * 0.9;
        const qreal kRotateTargetGap = KisTransformUtils::handleRadius + KisTransformUtils::handleGrabPadding + kRotateTargetRadius + 10.0;
        const QPointF centre = m_d->handlesTransform.map(m_d->bounds.center());
        const QPointF corners[4] = {
            m_d->handlesTransform.map(m_d->bounds.topLeft()),
            m_d->handlesTransform.map(m_d->bounds.topRight()),
            m_d->handlesTransform.map(m_d->bounds.bottomRight()),
            m_d->handlesTransform.map(m_d->bounds.bottomLeft())
        };
        QPainterPath targets;
        for (const QPointF &corner : corners) {
            QPointF out = corner - centre;
            const qreal len = std::hypot(out.x(), out.y());
            if (len < 1e-6) continue;
            out /= len;
            const QPointF c = corner + out * kRotateTargetGap;
            const qreal r = kRotateTargetRadius;
            const qreal facing = std::atan2(-out.y(), out.x()) * 180.0 / M_PI;
            const qreal start = facing - 50.0;
            const qreal sweep = 100.0;
            const QRectF circle(c.x() - r, c.y() - r, 2 * r, 2 * r);
            targets.arcMoveTo(circle, start);
            targets.arcTo(circle, start, sweep);
            const qreal endRad = (start + sweep) * M_PI / 180.0;
            const QPointF tip(c.x() + r * std::cos(endRad), c.y() - r * std::sin(endRad));
            const QPointF tangent(-std::sin(endRad), -std::cos(endRad));
            const QPointF normal(std::cos(endRad), -std::sin(endRad));
            const qreal head = r * 0.8;
            targets.moveTo(tip - tangent * head + normal * head * 0.6);
            targets.lineTo(tip);
            targets.lineTo(tip - tangent * head - normal * head * 0.6);
        }
        Q_UNUSED(targets);
    }

    for (int i = 1; i >= 0; --i) {
        gc.setPen(pen[i]);
        gc.drawPath(mappedHandles);
    }

    gc.restore();
}

void KisFreeTransformStrategy::externalConfigChanged()
{
    m_d->recalculateTransformations();
}

bool KisFreeTransformStrategy::beginPrimaryAction(const QPointF &pt)
{
    m_d->clickArgs = m_d->currentArgs;
    m_d->clickPos = pt;

    KisTransformUtils::MatricesPack m(m_d->clickArgs);
    m_d->clickTransform = m.finalTransform();

    if (m_d->function == ROTATEBOUNDS) {
        Q_EMIT requestConvexHullCalculation();
    }

    return true;
}

void KisFreeTransformStrategy::continuePrimaryAction(const QPointF &pt, bool shiftModifierActive, bool altModifierActive)
{
    KIS_SAFE_ASSERT_RECOVER_NOOP(0 && "this override is not used in KisFreeTransformStrategy");
}

void KisFreeTransformStrategy::continuePrimaryAction(const QPointF &mousePos,
                                                     bool shiftModifierActive,
                                                     bool altModifierActive,
                                                     bool hasOriginalModifiersSet)
{

    m_d->isTransforming = true;
    const QPointF anchorPoint = m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset();

    switch (m_d->function) {
    case MOVE: {
        QPointF diff = mousePos - m_d->clickPos;

        if (shiftModifierActive) {

            KisTransformUtils::MatricesPack m(m_d->clickArgs);
            QTransform t = m.S * m.projectedP;
            QPointF originalDiff = t.inverted().map(diff);

            if (qAbs(originalDiff.x()) >= qAbs(originalDiff.y())) {
                originalDiff.setY(0);
            } else {
                originalDiff.setX(0);
            }

            diff = t.map(originalDiff);

        }

        m_d->currentArgs.setTransformedCenter(m_d->clickArgs.transformedCenter() + diff);

        break;
    }
    case ROTATEBOUNDS:
    {
        const KisTransformUtils::MatricesPack clickM(m_d->clickArgs);
        const QTransform clickT = clickM.finalTransform();

        const QPointF rotationCenter = m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset();
        const QPointF clickMouseImagePos = clickT.inverted().map(m_d->clickPos) - rotationCenter;
        const QPointF mouseImagePosClickSpace = clickT.inverted().map(mousePos) - rotationCenter;

        const qreal a1 = atan2(clickMouseImagePos.y(), clickMouseImagePos.x());
        const qreal a2 = atan2(mouseImagePosClickSpace.y(), mouseImagePosClickSpace.x());

        const qreal theta = a2 - a1;
        m_d->currentArgs.setBoundsRotation(m_d->clickArgs.boundsRotation() + theta);

        QTransform newBR; newBR.rotateRadians(m_d->currentArgs.boundsRotation());
        QTransform clickZ; clickZ.rotateRadians(m_d->clickArgs.aZ());
        QTransform desired = newBR * clickM.BRI * clickM.SC * clickM.S * clickZ;
        KisAlgebra2D::DecomposedMatrix dm(desired);
        if (dm.isValid()) {
            m_d->currentArgs.setScaleX(dm.scaleX);
            m_d->currentArgs.setScaleY(dm.scaleY);
            m_d->currentArgs.setShearX(dm.shearXY);
            m_d->currentArgs.setShearY(0);
            m_d->currentArgs.setAZ(kisDegreesToRadians(dm.angle));
        }

    }
    break;
    case ROTATE:
    {
        const KisTransformUtils::MatricesPack clickM(m_d->clickArgs);
        const QTransform clickT = clickM.finalTransform();

        const QPointF rotationCenter = m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset();
        const QPointF clickMouseImagePos = clickT.inverted().map(m_d->clickPos) - rotationCenter;
        const QPointF mouseImagePosClickSpace = clickT.inverted().map(mousePos) - rotationCenter;

        const qreal a1 = atan2(clickMouseImagePos.y(), clickMouseImagePos.x());
        const qreal a2 = atan2(mouseImagePosClickSpace.y(), mouseImagePosClickSpace.x());

        const qreal theta = KisAlgebra2D::signZZ(clickM.SC.determinant()) * (a2 - a1);

        if (shiftModifierActive) {
            const qreal snapAngle = M_PI_4 / 6.0;
            qint32 thetaIndex = static_cast<qint32>((theta / snapAngle) + 0.5);
            m_d->currentArgs.setAZ(thetaIndex * snapAngle);
        }
        else {
            const qreal clickAngle = m_d->clickArgs.aZ();
            const qreal targetAngle = m_d->clickArgs.aZ() + theta;
            qreal shortestDistance = shortestAngularDistance(clickAngle, targetAngle);
            const bool clockwise =  (theta <= M_PI && theta >= 0) || (theta < -M_PI);
            shortestDistance = clockwise ? shortestDistance : shortestDistance * -1;

            m_d->currentArgs.setAZ(m_d->clickArgs.aZ() + shortestDistance);
        }

        KisTransformUtils::MatricesPack m(m_d->currentArgs);
        QTransform t = m.finalTransform();
        QPointF newRotationCenter = t.map(m_d->currentArgs.originalCenter() + m_d->currentArgs.rotationCenterOffset());
        QPointF oldRotationCenter = clickT.map(m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset());

        m_d->currentArgs.setTransformedCenter(m_d->currentArgs.transformedCenter() + oldRotationCenter - newRotationCenter);
    }
    break;
    case PERSPECTIVE:
    {
        if (!hasOriginalModifiersSet) {
            m_d->function = CAMERAHEIGHT;
            m_d->clickArgs = m_d->currentArgs;
            m_d->clickPos = mousePos;
            break;
        }

        QPointF diff = mousePos - m_d->clickPos;
        double thetaX = - diff.y() * M_PI / m_d->transaction.originalHalfHeight() / 2 / fabs(m_d->currentArgs.scaleY());
        m_d->currentArgs.setAX(normalizeAngle(m_d->clickArgs.aX() + thetaX));

        qreal sign = qAbs(m_d->currentArgs.aX() - M_PI) < M_PI / 2 ? -1.0 : 1.0;
        double thetaY = sign * diff.x() * M_PI / m_d->transaction.originalHalfWidth() / 2 / fabs(m_d->currentArgs.scaleX());
        m_d->currentArgs.setAY(normalizeAngle(m_d->clickArgs.aY() + thetaY));

        KisTransformUtils::MatricesPack m(m_d->currentArgs);
        QTransform t = m.finalTransform();
        QPointF newRotationCenter = t.map(m_d->currentArgs.originalCenter() + m_d->currentArgs.rotationCenterOffset());

        KisTransformUtils::MatricesPack clickM(m_d->clickArgs);
        QTransform clickT = clickM.finalTransform();
        QPointF oldRotationCenter = clickT.map(m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset());

        m_d->currentArgs.setTransformedCenter(m_d->currentArgs.transformedCenter() + oldRotationCenter - newRotationCenter);
    }
    break;
    case CAMERAHEIGHT: {
        if (hasOriginalModifiersSet) {
            m_d->function = PERSPECTIVE;
            m_d->clickPos = mousePos;
            break;
        }

        qreal dy = mousePos.y() - m_d->clickPos.y();
        m_d->clickPos = mousePos;

        qreal currentHeight = m_d->currentArgs.cameraPos().z();
        qreal currentHeightScaled = (currentHeight - 10150.0) / 9850.0;
        qreal heightDelta = -(currentHeightScaled * currentHeightScaled) + 1.001;

        const qreal sensitivityFactor = 0.02;
        qreal scaledDy = dy / sensitivityFactor;

        heightDelta = heightDelta * scaledDy;
        qreal newHeight = currentHeight + heightDelta;

        newHeight = qMax(300.0, qMin(20000.0, newHeight));
        m_d->currentArgs.setCameraPos(QVector3D(0, 0, newHeight));
    }
    break;
    case TOPSCALE:
    case BOTTOMSCALE: {
        QPointF staticPoint;
        QPointF movingPoint;

        if (m_d->function == TOPSCALE) {
            staticPoint = m_d->boundsTransform.map(bottomMiddle(m_d->bounds));
            movingPoint = m_d->boundsTransform.map(topMiddle(m_d->bounds));
        } else {
            staticPoint = m_d->boundsTransform.map(topMiddle(m_d->bounds));
            movingPoint = m_d->boundsTransform.map(bottomMiddle(m_d->bounds));
        }

        QPointF staticPointInView = m_d->clickTransform.map(staticPoint);
        const QPointF movingPointInView = m_d->clickTransform.map(movingPoint);

        const QPointF projNormVector =
            KisAlgebra2D::normalize(movingPointInView - staticPointInView);

        const qreal projLength =
            KisAlgebra2D::dotProduct(mousePos - staticPointInView, projNormVector);

        const QPointF targetMovingPointInView = staticPointInView + projNormVector * projLength;

        if ((m_d->clickArgs.transformAroundRotationCenter() ^ altModifierActive) &&
            !qFuzzyCompare(anchorPoint.y(), movingPoint.y())) {

            staticPoint = anchorPoint;
            staticPointInView = m_d->clickTransform.map(staticPoint);
        }

        GSL::ScaleResult1D result =
            GSL::calculateScaleY(m_d->currentArgs,
                                 staticPoint,
                                 staticPointInView,
                                 movingPoint,
                                 targetMovingPointInView);

        if (!result.isValid) {
            break;
        }

        if (shiftModifierActive ||  m_d->currentArgs.keepAspectRatio()) {
            qreal aspectRatio = m_d->clickArgs.scaleX() / m_d->clickArgs.scaleY();
            m_d->currentArgs.setScaleX(aspectRatio * result.scale);
        }

        m_d->currentArgs.setScaleY(result.scale);
        m_d->currentArgs.setTransformedCenter(result.transformedCenter);
        break;
    }

    case LEFTSCALE:
    case RIGHTSCALE: {
        QPointF staticPoint;
        QPointF movingPoint;

        if (m_d->function == LEFTSCALE) {
            staticPoint = m_d->boundsTransform.map(middleRight(m_d->bounds));
            movingPoint = m_d->boundsTransform.map(middleLeft(m_d->bounds));
        } else {
            staticPoint = m_d->boundsTransform.map(middleLeft(m_d->bounds));
            movingPoint = m_d->boundsTransform.map(middleRight(m_d->bounds));
        }

        QPointF staticPointInView = m_d->clickTransform.map(staticPoint);
        const QPointF movingPointInView = m_d->clickTransform.map(movingPoint);

        const QPointF projNormVector =
            KisAlgebra2D::normalize(movingPointInView - staticPointInView);

        const qreal projLength =
            KisAlgebra2D::dotProduct(mousePos - staticPointInView, projNormVector);

        const QPointF targetMovingPointInView = staticPointInView + projNormVector * projLength;

        if ((m_d->currentArgs.transformAroundRotationCenter() ^ altModifierActive) &&
            !qFuzzyCompare(anchorPoint.x(), movingPoint.x())) {

            staticPoint = anchorPoint;
            staticPointInView = m_d->clickTransform.map(staticPoint);
        }

        GSL::ScaleResult1D result =
            GSL::calculateScaleX(m_d->currentArgs,
                                 staticPoint,
                                 staticPointInView,
                                 movingPoint,
                                 targetMovingPointInView);

        if (!result.isValid) {
            break;
        }

        if (shiftModifierActive  ||  m_d->currentArgs.keepAspectRatio()) {
            qreal aspectRatio = m_d->clickArgs.scaleY() / m_d->clickArgs.scaleX();
            m_d->currentArgs.setScaleY(aspectRatio * result.scale);
        }

        m_d->currentArgs.setScaleX(result.scale);
        m_d->currentArgs.setTransformedCenter(result.transformedCenter);
        break;
    }
    case TOPRIGHTSCALE:
    case BOTTOMRIGHTSCALE:
    case TOPLEFTSCALE:
    case BOTTOMLEFTSCALE: {
        QPointF staticPoint;
        QPointF movingPoint;

        if (m_d->function == TOPRIGHTSCALE) {
            staticPoint = m_d->boundsTransform.map(m_d->bounds.bottomLeft());
            movingPoint = m_d->boundsTransform.map(m_d->bounds.topRight());
        } else if (m_d->function == BOTTOMRIGHTSCALE) {
            staticPoint = m_d->boundsTransform.map(m_d->bounds.topLeft());
            movingPoint = m_d->boundsTransform.map(m_d->bounds.bottomRight());
        } else if (m_d->function == TOPLEFTSCALE) {
            staticPoint = m_d->boundsTransform.map(m_d->bounds.bottomRight());
            movingPoint = m_d->boundsTransform.map(m_d->bounds.topLeft());
        } else {
            staticPoint = m_d->boundsTransform.map(m_d->bounds.topRight());
            movingPoint = m_d->boundsTransform.map(m_d->bounds.bottomLeft());
        }

        if ((m_d->currentArgs.transformAroundRotationCenter() ^ altModifierActive) &&
            !(qFuzzyCompare(anchorPoint.x(), movingPoint.x()) ||
              qFuzzyCompare(anchorPoint.y(), movingPoint.y()))) {

            staticPoint = anchorPoint;
        }

        QPointF staticPointInView = m_d->clickTransform.map(staticPoint);
        QPointF movingPointInView = mousePos;

        if (shiftModifierActive  ||  m_d->currentArgs.keepAspectRatio()) {
            QPointF refDiff = m_d->clickTransform.map(movingPoint) - staticPointInView;
            QPointF realDiff = mousePos - staticPointInView;
            realDiff = kisProjectOnVector(refDiff, realDiff);

            movingPointInView = staticPointInView + realDiff;
        }

        const bool isAffine =
            qFuzzyIsNull(m_d->currentArgs.aX()) &&
            qFuzzyIsNull(m_d->currentArgs.aY());

        GSL::ScaleResult2D result =
                !isAffine ?
                    GSL::calculateScale2D(m_d->currentArgs,
                                          staticPoint,
                                          staticPointInView,
                                          movingPoint,
                                          movingPointInView) :
                    GSL::calculateScale2DAffine(m_d->currentArgs,
                                                staticPoint,
                                                staticPointInView,
                                                movingPoint,
                                                movingPointInView);

        if (result.isValid) {
            m_d->currentArgs.setScaleX(result.scaleX);
            m_d->currentArgs.setScaleY(result.scaleY);
            m_d->currentArgs.setTransformedCenter(result.transformedCenter);
        }

        break;
    }
    case MOVECENTER: {

        QPointF pt;
        if (altModifierActive) {
            pt = (m_d->boundsTransform * m_d->transform).inverted().map(mousePos);
            pt = KisTransformUtils::clipInRect(pt, m_d->bounds);
            pt = m_d->boundsTransform.map(pt);
        } else {
            pt = m_d->transform.inverted().map(mousePos);
        }

        QPointF newRotationCenterOffset = pt - m_d->currentArgs.originalCenter();

        if (shiftModifierActive) {
            if (qAbs(newRotationCenterOffset.x()) > qAbs(newRotationCenterOffset.y())) {
                newRotationCenterOffset.ry() = 0;
            } else {
                newRotationCenterOffset.rx() = 0;
            }
        }

        m_d->currentArgs.setRotationCenterOffset(newRotationCenterOffset);
        Q_EMIT requestResetRotationCenterButtons();
    }
        break;
    case TOPSHEAR:
    case BOTTOMSHEAR: {
        KisTransformUtils::MatricesPack m(m_d->clickArgs);

        QPointF oldStaticPoint = m.finalTransform().map(m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset());

        QTransform backwardT = (m.S * m.projectedP).inverted();
        QPointF diff = backwardT.map(mousePos - m_d->clickPos);

        qreal sign = m_d->function == BOTTOMSHEAR ? 1.0 : -1.0;

        qreal dx = sign * m_d->clickArgs.shearX() * m_d->clickArgs.scaleY() * (m_d->bounds.height() / 2.0);
        dx += diff.x();

        m_d->currentArgs.setShearX(sign * dx / m_d->currentArgs.scaleY() / (m_d->bounds.height() / 2.0));

        KisTransformUtils::MatricesPack currentM(m_d->currentArgs);
        QTransform t = currentM.finalTransform();
        QPointF newStaticPoint = t.map(m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset());
        m_d->currentArgs.setTransformedCenter(m_d->currentArgs.transformedCenter() + oldStaticPoint - newStaticPoint);
        break;
    }

    case LEFTSHEAR:
    case RIGHTSHEAR: {
        KisTransformUtils::MatricesPack m(m_d->clickArgs);

        QPointF oldStaticPoint = m.finalTransform().map(m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset());

        QTransform backwardT = (m.S * m.projectedP).inverted();
        QPointF diff = backwardT.map(mousePos - m_d->clickPos);

        qreal sign = m_d->function == RIGHTSHEAR ? 1.0 : -1.0;

        qreal dy = sign *  m_d->clickArgs.shearY() * m_d->clickArgs.scaleX() * (m_d->bounds.width() / 2.0);
        dy += diff.y();

        m_d->currentArgs.setShearY(sign * dy / m_d->clickArgs.scaleX() / (m_d->bounds.width() / 2.0));

        KisTransformUtils::MatricesPack currentM(m_d->currentArgs);
        QTransform t = currentM.finalTransform();
        QPointF newStaticPoint = t.map(m_d->clickArgs.originalCenter() + m_d->clickArgs.rotationCenterOffset());
        m_d->currentArgs.setTransformedCenter(m_d->currentArgs.transformedCenter() + oldStaticPoint - newStaticPoint);
        break;
    }
    case KRIMBLENONE:
        break;
    }

    m_d->recalculateTransformations();
}

bool KisFreeTransformStrategy::endPrimaryAction()
{
    bool shouldSave = !m_d->imageTooBig;
    m_d->isTransforming = false;

    if (m_d->imageTooBig) {
        m_d->currentArgs = m_d->clickArgs;
        m_d->recalculateTransformations();
    }

    return shouldSave;
}

void KisFreeTransformStrategy::Private::recalculateTransformations()
{
    KisTransformUtils::MatricesPack m(currentArgs);
    QTransform sanityCheckMatrix = m.TS * m.SC * m.S * m.projectedP;

    KIS_ASSERT_RECOVER_NOOP(sanityCheckMatrix.map(currentArgs.originalCenter()).manhattanLength() < 1e-4);

    transform = m.finalTransform();
    boundsTransform = m.BRI.inverted();
    recalculateBounds();

    QTransform viewScaleTransform = converter->imageToDocumentTransform() * converter->documentToFlakeTransform();
    handlesTransform = boundsTransform * transform * viewScaleTransform;

    QTransform tl = QTransform::fromTranslate(transaction.originalTopLeft().x(), transaction.originalTopLeft().y());
    paintingTransform = tl.inverted() * q->thumbToImageTransform() * tl * transform * viewScaleTransform;
    paintingOffset = transaction.originalTopLeft();

    imageTooBig = KisTransformUtils::checkImageTooBig(transaction.originalRect(), m, currentArgs.cameraPos().z());

    recalculateTransformedHandles();

    Q_EMIT q->requestShowImageTooBig(imageTooBig);
    Q_EMIT q->requestImageRecalculation();
}
