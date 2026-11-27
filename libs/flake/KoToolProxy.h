/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006, 2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_TOOL_PROXY_H_
#define _KO_TOOL_PROXY_H_

#include "kritaflake_export.h"

#include <QObject>

class QAction;
class QAction;
class QMouseEvent;
class QKeyEvent;
class QWheelEvent;
class QTabletEvent;
class KoCanvasBase;
class KoViewConverter;
class KoToolBase;
class KoToolProxyPrivate;
class QInputMethodEvent;
class KoPointerEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QTouchEvent;
class QFocusEvent;
class QPainter;
class QPointF;
class QMenu;
class KisPopupWidgetInterface;

class KRITAFLAKE_EXPORT KoToolProxy : public QObject
{
    Q_OBJECT
public:
    explicit KoToolProxy(KoCanvasBase *canvas, QObject *parent = 0);
    ~KoToolProxy() override;

    void paint(QPainter &painter, const KoViewConverter &converter);

    void repaintDecorations();

    void tabletEvent(QTabletEvent *event, const QPointF &point);

    void mousePressEvent(QMouseEvent *event, const QPointF &point);
    void mousePressEvent(KoPointerEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point);
    void mouseDoubleClickEvent(KoPointerEvent *event);

    void mouseMoveEvent(QMouseEvent *event, const QPointF &point);
    void mouseMoveEvent(KoPointerEvent *event);

    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point);
    void mouseReleaseEvent(KoPointerEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void explicitUserStrokeEndRequest();

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    void inputMethodEvent(QInputMethodEvent *event);

    void focusInEvent(QFocusEvent *event);

    void focusOutEvent(QFocusEvent *event);

    QMenu* popupActionsMenu();

    KisPopupWidgetInterface* popupWidget();

    void deleteSelection();

    void processEvent(QEvent *) const;

    bool hasSelection() const;

    void cut();

    void copy() const;

    bool paste();

    bool selectAll();

    void deselect();

    void dragMoveEvent(QDragMoveEvent *event, const QPointF &point);

    void dragLeaveEvent(QDragLeaveEvent *event);

    void dropEvent(QDropEvent *event, const QPointF &point);

    virtual void setActiveTool(KoToolBase *tool);

    void touchEvent(QTouchEvent* event, const QPointF& point);

    KoPointerEvent* lastDeliveredPointerEvent() const;

    QVector<QKeySequence> toolPriorityShortcuts() const;

    KoToolProxyPrivate *priv();

protected Q_SLOTS:
    void requestUndoDuringStroke();

    void requestRedoDuringStroke();

    void requestStrokeCancellation();

    void requestStrokeEnd();

Q_SIGNALS:
    void selectionChanged(bool hasSelection);

    void toolChanged(const QString &toolId);

protected:
    virtual QPointF widgetToDocument(const QPointF &widgetPoint) const = 0;
    virtual QPointF documentToWidget(const QPointF &documentPoint) const = 0;
    KoCanvasBase* canvas() const;
    int multiClickCount() const;

private:
    Q_PRIVATE_SLOT(d, void timeout())
    Q_PRIVATE_SLOT(d, void selectionChanged(bool))

    void countMultiClick(KoPointerEvent *ev, int eventType);

    friend class KoToolProxyPrivate;
    KoToolProxyPrivate * const d;
};

#endif
