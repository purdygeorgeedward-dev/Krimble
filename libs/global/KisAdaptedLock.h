/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISADAPTEDLOCK_H
#define KISADAPTEDLOCK_H

#include <mutex>

template <typename Adapter>
class KisAdaptedLock
    : protected Adapter,
      public std::unique_lock<Adapter>
{
public:
    template<typename Object>
    KisAdaptedLock(Object object)
        : Adapter(object)
        , std::unique_lock<Adapter>(
              static_cast<Adapter&>(*this))
    {}

    template<typename Object>
    KisAdaptedLock(Object object, std::try_to_lock_t t)
        : Adapter(object)
        , std::unique_lock<Adapter>(static_cast<Adapter&>(*this), t)
    {}

    template<typename Object>
    KisAdaptedLock(Object object, std::defer_lock_t t)
        : Adapter(object)
        , std::unique_lock<Adapter>(static_cast<Adapter&>(*this), t)
    {}

    template<typename Object>
    KisAdaptedLock(Object object, std::adopt_lock_t t)
        : Adapter(object)
        , std::unique_lock<Adapter>(static_cast<Adapter&>(*this), t)
    {}

    KisAdaptedLock(KisAdaptedLock &&rhs)
        : Adapter(static_cast<Adapter&>(rhs))
        , std::unique_lock<Adapter>(
              static_cast<Adapter&>(*this), std::adopt_lock)
    {
        rhs.release();
    }

    KisAdaptedLock& operator=(KisAdaptedLock &&rhs)
    {
        static_cast<std::unique_lock<Adapter>&>(*this) =
            std::unique_lock<Adapter>(static_cast<Adapter&>(*this),
                                      std::adopt_lock);
        static_cast<Adapter&>(*this) = rhs;
        rhs.release();
        return *this;
    }

    using std::unique_lock<Adapter>::try_lock;
    using std::unique_lock<Adapter>::lock;
    using std::unique_lock<Adapter>::unlock;
    using std::unique_lock<Adapter>::owns_lock;
};

#define KIS_DECLARE_ADAPTED_LOCK(Name, Adapter) \
class Name : public KisAdaptedLock<Adapter>     \
{                                               \
    public:                                     \
    using BaseClass = KisAdaptedLock<Adapter>;  \
    using BaseClass::BaseClass;                 \
};

#endif
