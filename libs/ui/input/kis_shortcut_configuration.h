/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISSHORTCUTCONFIGURATION_H
#define KISSHORTCUTCONFIGURATION_H

#include <QList>
#include <QMetaType>

class QString;
class KisAbstractInputAction;

class KisShortcutConfiguration
{
public:
    enum ShortcutType {
        UnknownType,
        KeyCombinationType,
        MouseButtonType,
        MouseWheelType,
        GestureType,
        MacOSGestureType,
    };

    enum MouseWheelMovement {
        NoMovement,
        WheelUp,
        WheelDown,
        WheelLeft,
        WheelRight,
        WheelTrackpad,
    };

    enum GestureAction {
        NoGesture,
#ifdef Q_OS_MACOS
        PinchGesture,
        PanGesture,
        RotateGesture,
        SmartZoomGesture,
#else
        OneFingerTap,
        TwoFingerTap,
        ThreeFingerTap,
        FourFingerTap,
        FiveFingerTap,
        OneFingerDrag,
        TwoFingerDrag,
        ThreeFingerDrag,
        FourFingerDrag,
        FiveFingerDrag,
        OneFingerHold,
#endif
        MaxGesture,
    };

    KisShortcutConfiguration();
    KisShortcutConfiguration(const KisShortcutConfiguration &other);
    KisShortcutConfiguration &operator=(const KisShortcutConfiguration &other);

    bool operator==(const KisShortcutConfiguration &other) const;

    virtual ~KisShortcutConfiguration();

    QString serialize();
    bool unserialize(const QString &serialized);

    KisAbstractInputAction *action() const;
    void setAction(KisAbstractInputAction *newAction);

    ShortcutType type() const;
    void setType(ShortcutType newType);

    uint mode() const;
    void setMode(uint newMode);

    QList<Qt::Key> keys() const;
    void setKeys(const QList<Qt::Key> &newKeys);

    Qt::MouseButtons buttons() const;
    void setButtons(Qt::MouseButtons newButtons);

    MouseWheelMovement wheel() const;
    void setWheel(MouseWheelMovement type);

    GestureAction gesture() const;
    void setGesture(GestureAction type);

    bool isNoOp() const;

    QString getInputText() const;

    static QString buttonsToText(Qt::MouseButtons buttons);
    static QString keysToText(const QList<Qt::Key> &keys);
    static QString wheelToText(MouseWheelMovement wheel);
    static QString buttonsInputToText(const QList<Qt::Key> &keys, Qt::MouseButtons buttons);
    static QString wheelInputToText(const QList<Qt::Key> &keys, MouseWheelMovement wheel);

    static QString gestureToText(GestureAction action);

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KisShortcutConfiguration *);

#endif
