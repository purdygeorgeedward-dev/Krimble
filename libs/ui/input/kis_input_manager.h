/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_INPUTMANAGER_H
#define KIS_INPUTMANAGER_H

#include <QObject>
#include <QPointer>

#include <kritaui_export.h>

#include <kis_tool_proxy.h>

class QPointF;
class QTouchEvent;
class KisCanvas2;
class KRITAUI_EXPORT KisInputManager : public QObject
{
    Q_OBJECT

public:
    KisInputManager(QObject *parent);

    ~KisInputManager() override;

    void addTrackedCanvas(KisCanvas2 *canvas);
    void removeTrackedCanvas(KisCanvas2 *canvas);

    void registerPopupWidget(KisPopupWidgetInterface *popupWidget);

    void toggleTabletLogger();

    void setupAsEventFilter(QObject *receiver);

    bool eventFilter(QObject* object, QEvent* event ) override;

    void attachPriorityEventFilter(QObject *filter, int priority = 0);

    void detachPriorityEventFilter(QObject *filter);

    KisCanvas2 *canvas() const;

    QPointer<KisToolProxy> toolProxy() const;

private Q_SLOTS:
    void slotAboutToChangeTool();
    void slotToolChanged();
    void slotTextModeChanged();
    void profileChanged();
    void slotCompressedMoveEvent();
    void deregisterPopupWidget();
    void slotConfigChanged();
    void slotTouchHoldTriggered();

private:
    bool startTouch(bool &retval);
    void endTouch();
    bool touchHoldBufferUpdate(QTouchEvent *touchEvent);

    bool handleTouchBegin(QTouchEvent *touchEvent);
    bool handleTouchUpdate(QTouchEvent *touchEvent);

    bool eventFilterImpl(QEvent * event);
    template <class Event>
        bool compressMoveEventCommon(Event *event);

private:
    class Private;
    Private* const d;
};

#endif
