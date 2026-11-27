/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISSYNCHRONIZEDCONNECTION_H
#define KISSYNCHRONIZEDCONNECTION_H

#include <kritaglobal_export.h>

#include <QObject>
#include <QEvent>

#include <KisMpl.h>
#include <QMutex>
#include <QMutexLocker>
#include <QPointer>
#include <boost/bind/bind.hpp>
#include <functional>
#include <kis_assert.h>
#include <queue>

struct KRITAGLOBAL_EXPORT KisSynchronizedConnectionEvent : public QEvent
{
    KisSynchronizedConnectionEvent(QObject *_destination);
    KisSynchronizedConnectionEvent(const KisSynchronizedConnectionEvent &rhs);
    ~KisSynchronizedConnectionEvent() override;

    const QPointer<QObject> destination;
};

class KRITAGLOBAL_EXPORT KisSynchronizedConnectionBase : public QObject
{
public:
    static int eventType();
    static void registerSynchronizedEventBarrier(std::function<void()> callback);

    static void setAutoModeForUnittestsEnabled(bool value);
    static bool isAutoModeForUnittestsEnabled();

    static void forceDeliverAllSynchronizedEvents();

protected:
    bool event(QEvent *event) override;

protected:
    virtual void deliverEventToReceiver() = 0;
    void postEvent();
};

template <typename... Args>
class KisSynchronizedConnection : public KisSynchronizedConnectionBase
{
public:
public:
    using ArgsTuple = std::tuple<Args...>;
    using CallbackFunction = std::function<void (Args...)>;

public:
    KisSynchronizedConnection() = default;
    KisSynchronizedConnection(CallbackFunction callback)
        : m_callback(callback)
    {}

    void start(const Args &...argsTuple) {
        {
            QMutexLocker l(&m_inputConnectionMutex);
            m_queue.emplace(std::make_tuple(argsTuple...));
        }
        this->postEvent();
    }

    void setCallback(CallbackFunction callback) {
        m_callback = callback;
    }

    template <typename Dptr, typename C, typename R, typename ...MemFnArgs>
    void connectInputSignal(Dptr object, R (C::* memfn)(MemFnArgs...)) {
        static_assert (std::is_convertible<Dptr, const C*>::value, "Source object should be convertible into the base of the member pointer");
        static_assert (std::is_convertible<Dptr, const QObject*>::value, "Source object should be convertible into QObject");

        QObject::connect(static_cast<const C*>(object), memfn,
                         this, &KisSynchronizedConnection::start, Qt::DirectConnection);
    }

    template <typename Dptr, typename C, typename R, typename ...MemFnArgs>
    void connectOutputSlot(Dptr object, R (C::* memfn)(MemFnArgs...)) {
        static_assert (std::is_convertible<Dptr, C*>::value, "Destination object should be convertible into the base of the member pointer");
        KIS_SAFE_ASSERT_RECOVER_RETURN(!m_callback);

        m_callback = bindToMemberFunction(object, memfn,
                                          kismpl::make_index_sequence_from_1<
                                              std::tuple_size<ArgsTuple>::value>());
    }

    template <typename Dptr1, typename C1, typename R1, typename ...MemFnArgs1,
              typename Dptr2, typename C2, typename R2, typename ...MemFnArgs2>
    void connectSync(Dptr1 object1, R1 (C1::* memfn1)(MemFnArgs1...),
                     Dptr2 object2, R2 (C2::* memfn2)(MemFnArgs2...)) {

        connectInputSignal(object1, memfn1);
        connectOutputSlot(object2, memfn2);
    }

    bool hasPendingSignals() const {
        QMutexLocker l(&m_inputConnectionMutex);
        return !m_queue.empty();
    }

private:

    template <typename Dptr, typename C, typename R, typename ...MemFnArgs, std::size_t ...Idx>
    CallbackFunction bindToMemberFunction(Dptr object, R (C::* memfn)(MemFnArgs...), std::index_sequence<Idx...>) {

        return boost::bind(memfn, object, boost::arg<Idx>()...);
    }

protected:
    void deliverEventToReceiver() override {
        ArgsTuple args;

        {
            QMutexLocker l(&m_inputConnectionMutex);
            args = m_queue.front();
            m_queue.pop();
        }

        std::apply(m_callback, args);
    }

private:
    CallbackFunction m_callback;
    std::queue<ArgsTuple> m_queue;
    mutable QMutex m_inputConnectionMutex;
};

#endif
