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
#include <QPointer>

class QWidget;

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
    void continuePrimaryAction(KoPointerEvent *event) override;
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

    void touchUndoPoint();
    void touchClosePolygon();
    void touchCancelPolygon();

    void showStrokeBar(bool show);

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
    void advanceAnts();

private:

    QPointF m_dragStart;
    QPointF m_dragEnd;
    bool m_dragging;
    vQPointF m_points;
    ToolType m_type;
    bool m_closeSnappingActivated;

    QTimer *m_antsTimer {nullptr};
    qreal m_antsOffset {0.0};
    bool m_firstPointJustPlaced {false};   // the press that is going on put down the first point
    QPointF m_pressPoint;                  // where that press started (image pixels)
    QPointer<QWidget> m_strokeBar;
};

#endif // KIS_TOOL_POLYLINE_BASE_H
