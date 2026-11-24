/*
 *  SPDX-FileCopyrightText: 2006 Adrian Page <adrian@pagenet.plus.com>
 *  SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_INPUT_DEVICE_H_
#define KO_INPUT_DEVICE_H_

#include "kritaflake_export.h"

#include <QTabletEvent>
#include <QDebug>
#include <QFlags>
#include <boost/operators.hpp>

class KRITAFLAKE_EXPORT KoInputDevice : public boost::equality_comparable<KoInputDevice>
{
public:

    enum class Pointer {
        Unknown = 0,
        Generic = 0x0001,
        Finger = 0x0002,
        Pen = 0x0004,
        Eraser = 0x0008,
        Cursor = 0x0010,
        AllPointerTypes = 0x7FFF
    };

    enum class InputDevice {
        Unknown = 0x0000,
        Mouse = 0x0001,
        TouchScreen = 0x0002,
        TouchPad = 0x0004,
        Puck = 0x0008,
        Stylus = 0x0010,
        Airbrush = 0x0020,
        Keyboard = 0x1000,
        AllDevices = 0x7FFFFFFF
    };

    static Pointer convertPointerType(QTabletEvent *event)
    {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        switch (event->pointerType()) {
        case QTabletEvent::UnknownPointer: {
            return Pointer::Unknown;
        }
            break;
        case QTabletEvent::Pen: {
            return Pointer::Pen;
        }
            break;
        case QTabletEvent::Cursor: {
            return Pointer::Cursor;
        }
            break;
        case QTabletEvent::Eraser: {
            return Pointer::Eraser;
        }
        default:
            return Pointer::Unknown;
        }
#else
        QPointingDevice::PointerType pointer = event->pointerType();
        switch (pointer) {
        case (QPointingDevice::PointerType::Unknown): {
            return Pointer::Unknown;
        } break;
        case QPointingDevice::PointerType::Generic:{
            return Pointer::Generic;
        } break;
        case QPointingDevice::PointerType::Finger:{
            return Pointer::Finger;
        } break;
        case QPointingDevice::PointerType::Pen:{
            return Pointer::Pen;
        } break;
        case QPointingDevice::PointerType::Eraser:{
            return Pointer::Eraser;
        } break;

        case QPointingDevice::PointerType::Cursor:{
            return Pointer::Cursor;
        } break;
        case QPointingDevice::PointerType::AllPointerTypes:{
            return Pointer::AllPointerTypes;
        } break;
        default:
            return Pointer::Unknown;
        }
#endif
    }

    static InputDevice convertDeviceType(QTabletEvent *event)
    {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        switch (event->deviceType()) {
        case QTabletEvent::NoDevice: {
            return InputDevice::Unknown;
        }
            break;
        case QTabletEvent::Puck: {
            return InputDevice::Puck;
        }
            break;
        case QTabletEvent::Stylus: {
            return InputDevice::Stylus;
        }
            break;
        case QTabletEvent::Airbrush: {
            return InputDevice::Airbrush;
        }
        case QTabletEvent::RotationStylus: {
            return InputDevice::Stylus;
        }
        default:
            return InputDevice::Unknown;
        }

#else
        QInputDevice::DeviceType deviceType = event->deviceType();
        switch(deviceType) {
        case QInputDevice::DeviceType::Unknown: {
            return InputDevice::Unknown;
        }
        break;
    case QInputDevice::DeviceType::Mouse:{
            return InputDevice::Mouse;
        }
        break;
    case QInputDevice::DeviceType::TouchScreen:
        {
            return InputDevice::TouchScreen;
        }
        break;
    case QInputDevice::DeviceType::TouchPad:
        {
            return InputDevice::TouchPad;
        }
        break;
    case QInputDevice::DeviceType::Puck:
        {
            return InputDevice::Puck;
        }
        break;
    case QInputDevice::DeviceType::Stylus:
        {
            return InputDevice::Stylus;
        }
        break;
    case QInputDevice::DeviceType::Airbrush:
        {
            return InputDevice::Airbrush;
        }
        break;
    case QInputDevice::DeviceType::Keyboard:
        {
            return InputDevice::Keyboard;
        }
        break;
    case QInputDevice::DeviceType::AllDevices:
        {
            return InputDevice::AllDevices;
        }
    default:
            return InputDevice::Unknown;
    }
#endif
    }

    KoInputDevice(const KoInputDevice &other);

    explicit KoInputDevice(InputDevice device, Pointer pointer, qint64 uniqueTabletId = -1);

    KoInputDevice();

    ~KoInputDevice();

    InputDevice device() const;

    Pointer pointer() const;

    qint64 uniqueTabletId() const;

    bool isMouse() const;

    bool operator==(const KoInputDevice&) const;
    KoInputDevice & operator=(const KoInputDevice &);

    static KoInputDevice invalid();
    static KoInputDevice mouse();
    static KoInputDevice stylus();
    static KoInputDevice eraser();

private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(KoInputDevice)

KRITAFLAKE_EXPORT QDebug operator<<(QDebug debug, const KoInputDevice &device);

inline uint qHash(const KoInputDevice &key)
{
    return qHash(QString(":%1:%2:%3:%4")
                     .arg(int(key.device()))
                     .arg(int(key.pointer()))
                     .arg(int(key.uniqueTabletId()))
                     .arg(int(key.isMouse())));
}

#endif

