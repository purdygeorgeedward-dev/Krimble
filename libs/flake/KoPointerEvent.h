/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006 C. Boemann Rasmussen <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPOINTEREVENT_H
#define KOPOINTEREVENT_H

#include <QSharedPointer>
#include <QPointF>
#include <optional>

class QEvent;
class QTabletEvent;
class QMouseEvent;
class QWheelEvent;
class QTouchEvent;

#include "kritaflake_export.h"

struct KoPointerEventWrapper;

class KRITAFLAKE_EXPORT KoPointerEvent
{
public:
    KoPointerEvent(QMouseEvent *event, const QPointF &point);

    KoPointerEvent(QTabletEvent *event, const QPointF &point);

    KoPointerEvent(QTouchEvent* ev, const QPointF& pnt);

    KoPointerEvent(KoPointerEvent *event, const QPointF& point);

    ~KoPointerEvent();

    KoPointerEvent(const KoPointerEvent &rhs);

    KoPointerEvent& operator=(const KoPointerEvent &rhs);

    KoPointerEventWrapper deepCopyEvent() const;

    void accept();

    void ignore();

    Qt::KeyboardModifiers modifiers() const;

    bool isAccepted() const;

    bool spontaneous() const;

    Qt::MouseButton button() const;

    Qt::MouseButtons buttons() const;

    QPoint globalPos() const;

    QPoint pos() const;

    qreal pressure() const;

    qreal rotation() const;

    qreal tangentialPressure() const;

    int x() const;

    qreal xTilt() const;

    int y() const;

    qreal yTilt() const;

    int z() const;

    ulong time() const;

    QPointF point;

    bool isTabletEvent() const;

    bool isTouchEvent() const;

    static bool tabletInputReceived();

public:
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    static void copyQtPointerEvent(const QMouseEvent *event, QScopedPointer<QEvent> &dst);
    static void copyQtPointerEvent(const QTabletEvent *event, QScopedPointer<QEvent> &dst);
    static void copyQtPointerEvent(const QTouchEvent *event, QScopedPointer<QEvent> &dst);
#endif

    static std::optional<QPointF> fetchGlobalPositionFromPointerEvent(QEvent *event);

protected:
    friend class KoToolProxy;
    friend class KisToolProxy;
    friend class KisScratchPadEventFilter;
private:

    class Private;
    const QScopedPointer<Private> d;
};

struct KRITAFLAKE_EXPORT KoPointerEventWrapper
{
    template <typename Event>
    KoPointerEventWrapper(Event *_event, const QPointF &point);

    KoPointerEvent event;
    QSharedPointer<QEvent> baseQtEvent;
};

#endif

