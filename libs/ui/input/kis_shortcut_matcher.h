/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SHORTCUT_MATCHER_H
#define __KIS_SHORTCUT_MATCHER_H

#include "kis_single_action_shortcut.h"
#include "KisInputActionGroup.h"
#include <functional>
#include "kis_shortcut_configuration.h"

class QEvent;
class QWheelEvent;
class QTouchEvent;
class QNativeGestureEvent;
class QString;
class QPointF;

class KisStrokeShortcut;
class KisTouchShortcut;
class KisNativeGestureShortcut;

class KRITAUI_EXPORT KisShortcutMatcher
{
public:
    static constexpr int TOUCH_SLOP_SQUARED = 16 * 16;

    KisShortcutMatcher();
    ~KisShortcutMatcher();

    bool hasRunningShortcut() const;
    bool hasTouchHoldShortcut() const;

    void addShortcut(KisSingleActionShortcut *shortcut);
    void addShortcut(KisStrokeShortcut *shortcut);
    void addShortcut(KisTouchShortcut *shortcut);
    void addShortcut(KisNativeGestureShortcut *shortcut);

    bool supportsHiResInputEvents();

    bool keyPressed(Qt::Key key);

    bool autoRepeatedKeyPressed(Qt::Key key);

    bool keyReleased(Qt::Key key);

    bool buttonPressed(Qt::MouseButton button, QEvent *event);

    bool buttonReleased(Qt::MouseButton button, QEvent *event);

    bool wheelEvent(KisSingleActionShortcut::WheelAction wheelAction, QWheelEvent *event);

    bool pointerMoved(QEvent *event);

    void enterEvent();

    void leaveEvent();

    bool touchBeginEvent(QTouchEvent *event);
    bool touchUpdateEvent(QTouchEvent *event);
    bool touchEndEvent(QTouchEvent *event);

    void touchCancelEvent(QTouchEvent *event);

    bool touchHoldBeginEvent(QTouchEvent *event);

    bool nativeGestureBeginEvent(QNativeGestureEvent *event);
    bool nativeGestureEvent(QNativeGestureEvent *event);
    bool nativeGestureEndEvent(QNativeGestureEvent *event);

    void reinitialize();

    void reinitializeButtons();

    void handlePolledKeys(const QVector<Qt::Key> &keys);

    bool sanityCheckModifiersCorrectness(Qt::KeyboardModifiers modifiers) const;

    QVector<Qt::Key> debugPressedKeys() const;

    bool hasPolledKeys();

    void lostFocusEvent(const QPointF &localPos);

    void toolHasBeenActivated();

    void suppressAllActions(bool value);

    void suppressConflictingKeyActions(const QVector<QKeySequence> &shortcuts);

    void suppressAllKeyboardActions(bool value);

    void clearShortcuts();

    void setInputActionGroupsMaskCallback(std::function<KisInputActionGroupsMask()> func);

private:
    friend class KisInputManagerTest;

    void reset();
    void reset(QString msg);

    bool tryRunWheelShortcut(KisSingleActionShortcut::WheelAction wheelAction, QWheelEvent *event);
    template<typename T, typename U> bool tryRunSingleActionShortcutImpl(T param, U *event, const QSet<Qt::Key> &keysState, bool keyboard = true);

    void prepareReadyShortcuts();

    bool tryRunReadyShortcut( Qt::MouseButton button, QEvent* event );
    void tryActivateReadyShortcut();
    bool tryEndRunningShortcut( Qt::MouseButton button, QEvent* event );
    void forceEndRunningShortcut(const QPointF &localPos);
    void forceDeactivateAllActions();

    void setMaxTouchPointEvent(QTouchEvent *event);
    void fireReadyTouchShortcut(QTouchEvent *event);
    KisTouchShortcut *matchTouchShortcut(QTouchEvent *event);
    bool matchTouchShortcutBasedOnState(QTouchEvent *event, KisTouchShortcut *shortcut);
    bool tryRunTouchShortcut(QTouchEvent *event);
    bool tryEndTouchShortcut(QTouchEvent *event);

    bool tryRunNativeGestureShortcut(QNativeGestureEvent *event);
    bool tryEndNativeGestureShortcut(QNativeGestureEvent *event);

private:
    class Private;
    Private * const m_d;
};

#endif
