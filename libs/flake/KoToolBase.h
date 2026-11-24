/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTOOLBASE_H
#define KOTOOLBASE_H

#include <QObject>
#include <QPointer>
#include <QSet>
#include <QList>
#include <QHash>

#include <KisQStringListFwd.h>
#include "kritaflake_export.h"
#include "KoDerivedResourceConverter.h"
#include "KoAbstractCanvasResourceInterface.h"

class KoShape;
class KoCanvasBase;
class KoPointerEvent;
class KoViewConverter;
class KoToolFactoryBase;
class KoToolSelection;
class KoToolBasePrivate;
class KoShapeControllerBase;
class KisPopupWidgetInterface;

class QAction;
class QKeyEvent;
class QWidget;
class QCursor;
class QPainter;
class QString;
class QRectF;
class QPointF;
class QInputMethodEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QTouchEvent;
class QFocusEvent;
class QMenu;

class KRITAFLAKE_EXPORT KoToolBase : public QObject
{
    Q_OBJECT
public:
    explicit KoToolBase(KoCanvasBase *canvas);
    ~KoToolBase() override;

    KoToolFactoryBase* factory() const;

    virtual QRectF decorationsRect() const;

    virtual bool wantsAutoScroll() const;

    virtual void paint(QPainter &painter, const KoViewConverter &converter) = 0;

    QList<QPointer<QWidget> > optionWidgets();

    QAction *action(const QString &name) const;

    virtual void mousePressEvent(KoPointerEvent *event) = 0;

    virtual void mouseDoubleClickEvent(KoPointerEvent *event);

    virtual void mouseTripleClickEvent(KoPointerEvent *event);

    virtual void mouseMoveEvent(KoPointerEvent *event) = 0;

    virtual void mouseReleaseEvent(KoPointerEvent *event) = 0;

    virtual void keyPressEvent(QKeyEvent *event);

    virtual void keyReleaseEvent(QKeyEvent *event);

    virtual void explicitUserStrokeEndRequest();

    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    virtual void inputMethodEvent(QInputMethodEvent *event);

    virtual void focusInEvent(QFocusEvent *event);

    virtual void focusOutEvent(QFocusEvent *event);

    virtual void customPressEvent(KoPointerEvent *event);

    virtual void customReleaseEvent(KoPointerEvent *event);

    virtual void customMoveEvent(KoPointerEvent *event);

    bool maskSyntheticEvents() const;

    bool isOpacityPresetMode() const;

    Q_INVOKABLE QString toolId() const;

    QCursor cursor() const;

    virtual KoToolSelection *selection();

    virtual bool hasSelection();

    virtual void copy() const;

    virtual void deleteSelection();

    virtual void cut();

    virtual bool paste();

    virtual bool selectAll();

    virtual void deselect();

    virtual void dragMoveEvent(QDragMoveEvent *event, const QPointF &point);

    virtual void dragLeaveEvent(QDragLeaveEvent *event);

    virtual void dropEvent(QDropEvent *event, const QPointF &point);

    virtual QMenu* popupActionsMenu() {return nullptr;}

    virtual KisPopupWidgetInterface* popupWidget() {return nullptr;}

    KoCanvasBase *canvas() const;

    bool isInTextMode() const;

    int decorationThickness() const;

public Q_SLOTS:

    virtual void requestUndoDuringStroke();

    virtual void requestRedoDuringStroke();

    virtual void requestStrokeCancellation();

    virtual void requestStrokeEnd();

    virtual void activate(const QSet<KoShape*> &shapes);

    virtual void deactivate();

    virtual void canvasResourceChanged(int key, const QVariant &res);

    virtual void documentResourceChanged(int key, const QVariant &res);

    void setStatusText(const QString &statusText);

    virtual void repaintDecorations();

    void updateOptionsWidgetIcons();

Q_SIGNALS:

    void activateTool(const QString &id);

    void cursorChanged(const QCursor &cursor);

    void selectionChanged(bool hasSelection);

    void statusTextChanged(const QString &statusText);

    void textModeChanged(bool inTextMode);

protected:
    void useCursor(const QCursor &cursor);

    virtual QWidget *createOptionWidget();
    virtual QList<QPointer<QWidget> > createOptionWidgets();

    int handleRadius() const;

    qreal handleDocRadius() const;

    int grabSensitivity() const;

    QRectF handleGrabRect(const QPointF &position) const;

    QRectF handlePaintRect(const QPointF &position) const;

    void setTextMode(bool value);

    void setMaskSyntheticEvents(bool value);

    void setIsOpacityPresetMode(bool value);

    void setConverter(KoDerivedResourceConverterSP converter);

    void setAbstractResource(KoAbstractCanvasResourceInterfaceSP abstractResource);

    QHash<int, KoAbstractCanvasResourceInterfaceSP> toolAbstractResources();
    QHash<int, KoDerivedResourceConverterSP> toolConverters();

    bool isActivated() const;

    KoPointerEvent* lastDeliveredPointerEvent() const;

protected:
    KoToolBase(KoToolBasePrivate &dd);

    KoToolBasePrivate *d_ptr;

private:

    friend class KoToolManager;

    void setFactory(KoToolFactoryBase *factory);

    KoToolBase();
    KoToolBase(const KoToolBase&);
    KoToolBase& operator=(const KoToolBase&);

    Q_DECLARE_PRIVATE(KoToolBase)
};

#endif
