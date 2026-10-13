/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_TOOL_POLYLINE_BASE_H
#define KIS_TOOL_POLYLINE_BASE_H

#include <kis_tool_shape.h>
#include <kis_cursor.h>
#include <QTimer>

class KRITAUI_EXPORT KisToolPolylineBase : public KisToolShape
{
Q_OBJECT
public:
    enum ToolType {
        PAINT,
        SELECT
    };

    KisToolPolylineBase(KoCanvasBase * canvas, KisToolPolylineBase::ToolType type, const QCursor & cursor=KisCursor::load("tool_polygon_cursor.png", 6, 6));

    void beginPrimaryAction(KoPointerEvent *event) override;
    void continuePrimaryAction(KoPointerEvent *event) override;   // Krimble 2026-10-13: the point follows the finger
    void endPrimaryAction(KoPointerEvent *event) override;
    void beginPrimaryDoubleClickAction(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    void beginAlternateAction(KoPointerEvent *event, AlternateAction action) override;

    void paint(QPainter& gc, const KoViewConverter &converter) override;

    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;
    void requestStrokeEnd() override;
    void requestStrokeCancellation() override;
    KisPopupWidgetInterface* popupWidget() override;

    // Krimble 2026-10-13: what the buttons in Tool Options (Undo Point, Close, Cancel) do, for a finger that has no Enter or Escape key
    void touchUndoPoint();
    void touchClosePolygon();
    void touchCancelPolygon();

protected:
    virtual void finishPolyline(const QVector<QPointF>& points) = 0;

private:
    void endStroke();
    void cancelStroke();
    void updateArea();
    QRectF dragBoundingRect();

private Q_SLOTS:
    void undoSelection();
    void undoSelectionOrCancel();
    void advanceAnts();   // Krimble 2026-10-13: moves the marching ants one step

private:

    QPointF m_dragStart;
    QPointF m_dragEnd;
    bool m_dragging;
    vQPointF m_points;
    ToolType m_type;
    bool m_closeSnappingActivated;

    // Krimble 2026-10-13 (George's design): on a touch screen the polygon is built point by point with a finger, with marching ants
    QTimer *m_antsTimer {nullptr};
    qreal m_antsOffset {0.0};
    bool m_firstPointJustPlaced {false};   // the press that is going on put down the first point
    QPointF m_pressPoint;                  // where that press started (image pixels)
};

#endif // KIS_TOOL_POLYLINE_BASE_H
