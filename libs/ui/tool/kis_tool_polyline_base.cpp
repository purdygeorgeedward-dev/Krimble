/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */


#include <QAction>

#include <KoPointerEvent.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoViewConverter.h>
#include <input/kis_input_manager.h>

#include "kis_tool_polyline_base.h"
#include "kis_canvas2.h"
#include <kis_canvas_resource_provider.h>
#include <KisViewManager.h>
#include <kis_action.h>
#include <kactioncollection.h>
#include <kis_icon.h>

#include "kis_action_registry.h"

#define SNAPPING_THRESHOLD 10
#define SNAPPING_HANDLE_RADIUS 8
#define PREVIEW_LINE_WIDTH 1
// Krimble 2026-10-12: on a touch screen the polygon is closed by tapping near its first point, within this many view px (a finger is
// not a mouse pointer: nothing hovers before the tap, so the close check cannot wait for a mouse move).
#define SNAPPING_THRESHOLD_TOUCH 40
#define SNAPPING_RING_RADIUS_TOUCH 16
// Krimble 2026-10-13: touch polygon (George's design). Distances are in view px (logical).
#define TOUCH_CLOSE_DISTANCE 30      // dropping a point this close to the first point closes the polygon
#define TOUCH_TAP_DISTANCE 12        // a press of the first point that moves less than this is a tap (the first point only)
#define TOUCH_ANCHOR_RADIUS 6
#define TOUCH_ANTS_DASH 4

KisToolPolylineBase::KisToolPolylineBase(KoCanvasBase * canvas,  KisToolPolylineBase::ToolType type, const QCursor & cursor)
    : KisToolShape(canvas, cursor),
      m_dragging(false),
      m_type(type),
      m_closeSnappingActivated(false)
{
    KisCanvas2 *kritaCanvas = dynamic_cast<KisCanvas2*>(canvas);

    connect(kritaCanvas->viewManager()->canvasResourceProvider(), SIGNAL(sigEffectiveCompositeOpChanged()), SLOT(resetCursorStyle()));

    m_antsTimer = new QTimer(this);
    m_antsTimer->setInterval(90);
    connect(m_antsTimer, SIGNAL(timeout()), SLOT(advanceAnts()));
}


void KisToolPolylineBase::activate(const QSet<KoShape *> &shapes)
{
    KisToolShape::activate(shapes);
    connect(action("undo_polygon_selection"), SIGNAL(triggered()), SLOT(undoSelectionOrCancel()), Qt::UniqueConnection);

    KisInputManager *inputManager = (static_cast<KisCanvas2*>(canvas()))->globalInputManager();
    if (inputManager) {
        inputManager->attachPriorityEventFilter(this);
    }
}

void KisToolPolylineBase::deactivate()
{
    disconnect(action("undo_polygon_selection"), 0, this, 0);
    cancelStroke();

    KisInputManager *inputManager = (static_cast<KisCanvas2*>(canvas()))->globalInputManager();
    if (inputManager) {
        inputManager->detachPriorityEventFilter(this);
    }

    KisToolShape::deactivate();
}

void KisToolPolylineBase::requestStrokeEnd()
{
    endStroke();
}

void KisToolPolylineBase::requestStrokeCancellation()
{
    cancelStroke();
}

KisPopupWidgetInterface* KisToolPolylineBase::popupWidget()
{
    return m_dragging || m_type == SELECT ? nullptr : KisToolShape::popupWidget();
}

// Install an event filter to catch right-click events.
// The simplest way to accommodate the popup palette binding.
bool KisToolPolylineBase::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (!m_dragging) {
        return false;
    }
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            undoSelectionOrCancel();
            return true;
        }
    } else if (event->type() == QEvent::TabletPress) {
        QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);
        if (tabletEvent->button() == Qt::RightButton) {
            undoSelectionOrCancel();
            return true;
        }
    }
    return false;
}

void KisToolPolylineBase::beginPrimaryAction(KoPointerEvent *event)
{
    Q_UNUSED(event);
    NodePaintAbility paintability = nodePaintAbility();
    if ((m_type == PAINT && (!nodeEditable() || paintability == UNPAINTABLE || paintability  == KisToolPaint::CLONE || paintability == KisToolPaint::MYPAINTBRUSH_UNPAINTABLE)) ||
        (m_type == SELECT && !selectionEditable())) {

        if (paintability == KisToolPaint::CLONE){
            KisCanvas2 * kiscanvas = static_cast<KisCanvas2*>(canvas());
            QString message = i18n("This tool cannot paint on clone layers.  Please select a paint or vector layer or mask.");
            kiscanvas->viewManager()->showFloatingMessage(message, koIcon("object-locked"));
        }

        if (paintability == KisToolPaint::MYPAINTBRUSH_UNPAINTABLE) {
            KisCanvas2 * kiscanvas = static_cast<KisCanvas2*>(canvas());
            QString message = i18n("The MyPaint Brush Engine is not available for this colorspace");
            kiscanvas->viewManager()->showFloatingMessage(message, koIcon("object-locked"));
        }

        event->ignore();
        return;
    }

    setMode(KisTool::PAINT_MODE);

#ifdef Q_OS_ANDROID
    // Krimble 2026-10-13: George's design for the polygon on a touch screen: "the first touch lays the first point. Then you drag the
    // next point with a visible marching ants effect on the line. That drag could start immediately or with a subsequent tap. When you
    // release it sets that next point ... until a point is dropped within a few pixels of the first point, and then it ends and
    // closes the selection area."
    // - a press with no points yet puts down the first point at once;
    // - every press starts a rubber band (marching ants) from the last point to the finger, which follows the finger;
    // - releasing puts the next point down (endPrimaryAction), or closes the polygon if it is dropped close to the first point.
    {
        const QPointF pressPoint = convertToPixelCoordAndSnap(event);
        beginShape();
        m_dragging = true;
        m_firstPointJustPlaced = false;
        if (m_points.empty()) {
            m_points.append(pressPoint);
            m_firstPointJustPlaced = true;
            if (m_type == SELECT) {
                if (KisCanvas2 *kiscanvas = dynamic_cast<KisCanvas2*>(canvas())) {
                    kiscanvas->viewManager()->showFloatingMessage(i18n("Tap or drag to put down each point. Drop a point on the first one to close the selection."), QIcon());
                }
            }
        }
        m_pressPoint = pressPoint;
        m_dragStart = m_points.last();
        m_dragEnd = pressPoint;
        m_closeSnappingActivated = false;
        if (m_antsTimer && !m_antsTimer->isActive()) m_antsTimer->start();
        updateCanvasViewRect(pixelToView(QRectF(m_dragStart, m_dragEnd).normalized()).adjusted(-40, -40, 40, 40));
        return;
    }
#endif

#if 0
    // (the first version of the touch polygon, 2026-10-12: a tap near the first point closes it. Kept here as it was.)
    if (m_dragging && m_points.size() > 2) {
        const QPointF basePoint = pixelToView(m_points.first());
        const QPointF tapPoint = pixelToView(convertToPixelCoordAndSnap(event));
        if ((basePoint - tapPoint).manhattanLength() < SNAPPING_THRESHOLD_TOUCH) {
            m_closeSnappingActivated = true;
        }
    }
#endif   // #if 0

    if(m_dragging && m_closeSnappingActivated) {
        m_points.append(m_points.first());
        endStroke();
    } else {
        beginShape();
        m_dragging = true;
#ifdef Q_OS_ANDROID
        if (m_type == SELECT && m_points.empty()) {
            if (KisCanvas2 *kiscanvas = dynamic_cast<KisCanvas2*>(canvas())) {
                kiscanvas->viewManager()->showFloatingMessage(i18n("Tap each corner. Tap the first point again to close the selection."), QIcon());
            }
        }
#endif
    }
}

void KisToolPolylineBase::endPrimaryAction(KoPointerEvent *event)
{
    CHECK_MODE_SANITY_OR_RETURN(KisTool::PAINT_MODE);
    setMode(KisTool::HOVER_MODE);

#ifdef Q_OS_ANDROID
    // Krimble 2026-10-13 (George's design): releasing puts the next point down where the finger is; dropping it close to the first
    // point (once there are at least three points) closes the polygon and makes the selection. A press that only put down the first
    // point and did not move is just that point.
    if (m_dragging) {
        const QPointF releasePoint = convertToPixelCoordAndSnap(event);
        const qreal movedFromPress = (pixelToView(releasePoint) - pixelToView(m_pressPoint)).manhattanLength();
        if (m_firstPointJustPlaced && movedFromPress < TOUCH_TAP_DISTANCE) {
            // only the first point
        } else if (m_points.size() >= 3
                   && (pixelToView(m_points.first()) - pixelToView(releasePoint)).manhattanLength() < TOUCH_CLOSE_DISTANCE) {
            m_firstPointJustPlaced = false;
            m_points.append(m_points.first());
            endStroke();
            return;
        } else {
            m_points.append(releasePoint);
        }
        m_firstPointJustPlaced = false;
        m_dragStart = m_points.last();
        m_dragEnd = m_dragStart;
        m_closeSnappingActivated = false;
        updateCanvasViewRect(pixelToView(QRectF(m_points.first(), m_points.last()).normalized()).adjusted(-60, -60, 60, 60));
        return;
    }
#endif

    if(m_dragging) {
        m_dragStart = convertToPixelCoordAndSnap(event);
        m_dragEnd = m_dragStart;
        m_points.append(m_dragStart);
    }
}

void KisToolPolylineBase::continuePrimaryAction(KoPointerEvent *event)
{
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-13 (George's design): while the finger is down, the line from the last point follows it (with marching ants), and
    // the first point lights up when the finger is close enough to close the polygon.
    if (m_dragging && !m_points.empty()) {
        QRectF updateRect = pixelToView(QRectF(m_dragStart, m_dragEnd).normalized());
        m_dragEnd = convertToPixelCoordAndSnap(event);
        updateRect |= pixelToView(QRectF(m_dragStart, m_dragEnd).normalized());
        m_closeSnappingActivated = m_points.size() >= 3
            && (pixelToView(m_points.first()) - pixelToView(m_dragEnd)).manhattanLength() < TOUCH_CLOSE_DISTANCE;
        const QPointF basePoint = pixelToView(m_points.first());
        updateRect |= QRectF(basePoint, QSizeF(1, 1)).adjusted(-40, -40, 40, 40);
        updateCanvasViewRect(updateRect.adjusted(-40, -40, 40, 40));
        return;
    }
#endif
    KisToolShape::continuePrimaryAction(event);
}

void KisToolPolylineBase::advanceAnts()
{
    m_antsOffset += 1.0;
    if (!m_dragging || m_points.empty()) return;
    QRectF all(m_points.first(), m_points.first());
    Q_FOREACH (const QPointF &p, m_points) {
        all = all.united(QRectF(p, p));
    }
    all = all.united(QRectF(m_dragEnd, m_dragEnd));
    updateCanvasViewRect(pixelToView(all).adjusted(-30, -30, 30, 30));
}

void KisToolPolylineBase::beginPrimaryDoubleClickAction(KoPointerEvent *event)
{
    endStroke();

    // this action will have no continuation
    event->ignore();
}

void KisToolPolylineBase::beginAlternateAction(KoPointerEvent *event, AlternateAction action)
{
    if ((action != ChangeSize && action != ChangeSizeSnap) || !m_dragging) {
        KisToolPaint::beginAlternateAction(event, action);
    }

    if (m_closeSnappingActivated) {
        m_points.append(m_points.first());
    }
    endStroke();
}

void KisToolPolylineBase::mouseMoveEvent(KoPointerEvent *event)
{
    if (m_dragging && !m_points.empty()) {
        // erase old lines on canvas
        QRectF updateRect = dragBoundingRect();
        // get current mouse position
        m_dragEnd = convertToPixelCoordAndSnap(event);
        // draw new lines on canvas
        updateRect |= dragBoundingRect();
        updateCanvasViewRect(updateRect);


        QPointF basePoint = pixelToView(m_points.first());
        m_closeSnappingActivated =
            m_points.size() > 1 &&
            (basePoint - pixelToView(m_dragEnd)).manhattanLength() < SNAPPING_THRESHOLD;

        updateCanvasViewRect(QRectF(basePoint, 2 * QSize(SNAPPING_HANDLE_RADIUS + PREVIEW_LINE_WIDTH, SNAPPING_HANDLE_RADIUS + PREVIEW_LINE_WIDTH)).translated(-SNAPPING_HANDLE_RADIUS + PREVIEW_LINE_WIDTH,-SNAPPING_HANDLE_RADIUS + PREVIEW_LINE_WIDTH));
        KisToolPaint::requestUpdateOutline(event->point, event);
    } else {
        KisToolPaint::mouseMoveEvent(event);
    }
}

void KisToolPolylineBase::undoSelection()
{
    if (m_dragging) {
        // Initialize with the dragging segment's rect
        QRectF updateRect = dragBoundingRect();

        if (m_points.size() > 1) {
            // Add the rect for the last segment
            const QRectF lastSegmentRect =
                pixelToView(QRectF(m_points.last(), m_points.at(m_points.size() - 2)).normalized())
                .adjusted(-PREVIEW_LINE_WIDTH, -PREVIEW_LINE_WIDTH, PREVIEW_LINE_WIDTH, PREVIEW_LINE_WIDTH);
            updateRect = updateRect.united(lastSegmentRect);

            m_points.pop_back();
        }
        m_dragStart = m_points.last();

        // Add the new dragging segment's rect
        updateRect = updateRect.united(dragBoundingRect());
        updateCanvasViewRect(updateRect);
    }
}

// Krimble 2026-10-13: the Tool Options buttons.
void KisToolPolylineBase::touchUndoPoint()
{
    if (!m_dragging) return;
    if (m_points.size() > 1) {
        undoSelection();
        m_dragEnd = m_points.last();
        m_closeSnappingActivated = false;
        updateArea();
    } else {
        cancelStroke();     // the only point that was left
    }
}

void KisToolPolylineBase::touchClosePolygon()
{
    if (!m_dragging) return;
    if (m_points.size() >= 3) {
        m_points.append(m_points.first());
        endStroke();
    } else if (KisCanvas2 *kiscanvas = dynamic_cast<KisCanvas2*>(canvas())) {
        kiscanvas->viewManager()->showFloatingMessage(i18n("A polygon needs at least three points."), QIcon());
    }
}

void KisToolPolylineBase::touchCancelPolygon()
{
    cancelStroke();
}

void KisToolPolylineBase::undoSelectionOrCancel()
{
    if (m_points.size() > 1) {
        undoSelection();
    } else {
        cancelStroke();
    }
}

void KisToolPolylineBase::paint(QPainter& gc, const KoViewConverter &converter)
{
    Q_UNUSED(converter);

    if (!canvas() || !currentImage())
        return;

#ifdef Q_OS_ANDROID
    // Krimble 2026-10-13 (George's design): the selection being built is drawn with marching ants (a black and a white dashed line that
    // slide along it), with a round anchor at every point, and the first point is ringed when the polygon can be closed.
    if (m_type == SELECT && m_dragging && !m_points.empty()) {
        QPainterPath ants;
        ants.moveTo(pixelToView(m_points.first()));
        for (int i = 1; i < m_points.size(); ++i) {
            ants.lineTo(pixelToView(m_points.at(i)));
        }
        ants.lineTo(pixelToView(m_dragEnd));
        gc.save();
        gc.setRenderHint(QPainter::Antialiasing, true);
        QPen blackPen(Qt::black, 1.5);
        blackPen.setDashPattern(QVector<qreal>() << TOUCH_ANTS_DASH << TOUCH_ANTS_DASH);
        blackPen.setDashOffset(m_antsOffset);
        QPen whitePen(Qt::white, 1.5);
        whitePen.setDashPattern(QVector<qreal>() << TOUCH_ANTS_DASH << TOUCH_ANTS_DASH);
        whitePen.setDashOffset(m_antsOffset + TOUCH_ANTS_DASH);
        gc.setBrush(Qt::NoBrush);
        gc.setPen(blackPen);
        gc.drawPath(ants);
        gc.setPen(whitePen);
        gc.drawPath(ants);
        // the way back to the first point, fainter, so the shape that would be selected can be seen
        if (m_points.size() >= 2) {
            QPen closePen(QColor(255, 255, 255, 110), 1.0);
            closePen.setStyle(Qt::DotLine);
            gc.setPen(closePen);
            gc.drawLine(pixelToView(m_dragEnd), pixelToView(m_points.first()));
        }
        gc.setPen(QPen(Qt::black, 1.5));
        gc.setBrush(Qt::white);
        for (int i = 0; i < m_points.size(); ++i) {
            gc.drawEllipse(pixelToView(m_points.at(i)), TOUCH_ANCHOR_RADIUS, TOUCH_ANCHOR_RADIUS);
        }
        if (m_points.size() >= 3) {
            gc.setBrush(Qt::NoBrush);
            gc.setPen(QPen(m_closeSnappingActivated ? QColor(255, 200, 0) : QColor(255, 255, 255, 170), m_closeSnappingActivated ? 3.0 : 1.5));
            gc.drawEllipse(pixelToView(m_points.first()), SNAPPING_RING_RADIUS_TOUCH, SNAPPING_RING_RADIUS_TOUCH);
        }
        gc.restore();
        KisToolPaint::paint(gc, converter);
        return;
    }
#endif

    QPointF start, end;
    QPointF startPos;
    QPointF endPos;

    QPainterPath path;
    if (m_dragging && !m_points.empty()) {
        startPos = pixelToView(m_dragStart);
        endPos = pixelToView(m_dragEnd);
        path.moveTo(startPos);
        path.lineTo(endPos);
    }

    for (vQPointF::iterator it = m_points.begin(); it != m_points.end(); ++it) {

        if (it == m_points.begin()) {
            start = (*it);
        } else {
            end = (*it);

            startPos = pixelToView(start);
            endPos = pixelToView(end);
            path.moveTo(startPos);
            path.lineTo(endPos);
            start = end;
        }
    }

    if (m_closeSnappingActivated) {
        QPointF basePoint = pixelToView(m_points.first());
        path.addEllipse(basePoint, SNAPPING_HANDLE_RADIUS, SNAPPING_HANDLE_RADIUS);
    }
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-12: a ring on the first point once the polygon can be closed, so it is clear where to tap.
    else if (m_dragging && m_points.size() > 2) {
        path.addEllipse(pixelToView(m_points.first()), SNAPPING_RING_RADIUS_TOUCH, SNAPPING_RING_RADIUS_TOUCH);
    }
#endif

    paintToolOutline(&gc, path);
    KisToolPaint::paint(gc,converter);
}

void KisToolPolylineBase::updateArea()
{
    updateCanvasPixelRect(image()->bounds());
}

void KisToolPolylineBase::endStroke()
{
    if (!m_dragging) return;

    m_dragging = false;
    if (m_antsTimer) m_antsTimer->stop();
    if(m_points.count() > 1) {
        finishPolyline(m_points);
    }
    m_points.clear();
    m_closeSnappingActivated = false;
    updateArea();
    endShape();
}

void KisToolPolylineBase::cancelStroke()
{
    if (!m_dragging) return;

    m_dragging = false;
    if (m_antsTimer) m_antsTimer->stop();
    m_points.clear();
    m_closeSnappingActivated = false;
    updateArea();
    endShape();
}

QRectF KisToolPolylineBase::dragBoundingRect()
{
    QRectF rect = pixelToView(QRectF(m_dragStart, m_dragEnd).normalized());
    rect.adjust(-PREVIEW_LINE_WIDTH, -PREVIEW_LINE_WIDTH, PREVIEW_LINE_WIDTH, PREVIEW_LINE_WIDTH);
    return rect;
}
