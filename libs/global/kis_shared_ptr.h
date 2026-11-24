/*
 *  SPDX-FileCopyrightText: 2005 Frerich Raabe <raabe@kde.org>
 *  SPDX-FileCopyrightText: 2006, 2010 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SHAREDPTR_H
#define KIS_SHAREDPTR_H

#include <QtGlobal>

#include <kis_debug.h>

#include "kis_memory_leak_tracker.h"

template<class T>
class KisWeakSharedPtr;

template<class T>
class KisSharedPtr
{
    friend class KisWeakSharedPtr<T>;
public:
    using element_type = T;
    using weak_type = KisWeakSharedPtr<T>;

    inline KisSharedPtr()
            : d(0) { }

    inline KisSharedPtr(T* p)
            : d(p) {
        ref();
    }

    inline KisSharedPtr(const KisWeakSharedPtr<T>& o);

    void attach(T* p);

    void clear();

    inline KisSharedPtr(const KisSharedPtr<T>& o)
            : d(o.d) {
        ref();
    }

    inline ~KisSharedPtr() {
        deref();
    }

    inline KisSharedPtr& operator= (const KisSharedPtr& o) {
        attach(o.d);
        return *this;
    }
    inline bool operator== (const T* p) const {
        return (d == p);
    }
    inline bool operator!= (const T* p) const {
        return (d != p);
    }
    inline bool operator== (const KisSharedPtr& o) const {
        return (d == o.d);
    }
    inline bool operator!= (const KisSharedPtr& o) const {
        return (d != o.d);
    }

    inline KisSharedPtr<T>& operator= (T* p) {
        attach(p);
        return *this;
    }

    inline operator const T*() const {
        return d;
    }

    template< class T2> inline operator KisSharedPtr<T2>() const {
        return KisSharedPtr<T2>(d);
    }

    inline T* data() {
        return d;
    }

    inline const T* data() const {
        return d;
    }

    inline const T* constData() const {
        return d;
    }

    inline const T& operator*() const {
        Q_ASSERT(d);
        return *d;
    }
    inline T& operator*() {
        Q_ASSERT(d);
        return *d;
    }

    inline const T* operator->() const {
        Q_ASSERT(d);
        return d;
    }
    inline T* operator->() {
        Q_ASSERT(d);
        return d;
    }

    inline bool isNull() const {
        return (d == 0);
    }

    inline static void ref(const KisSharedPtr<T>* sp, T* t)
    {
#ifndef HAVE_MEMORY_LEAK_TRACKER
        Q_UNUSED(sp);
#else
        KisMemoryLeakTracker::instance()->reference(t, sp);
#endif
        if (t) {
            t->ref();
        }
    }

    inline static bool deref(const KisSharedPtr<T>* sp, T* t)
    {
#ifndef HAVE_MEMORY_LEAK_TRACKER
        Q_UNUSED(sp);
#else
        KisMemoryLeakTracker::instance()->dereference(t, sp);
#endif
        if (t && !t->deref()) {
            delete t;
            return false;
        }
        return true;
    }

private:
    inline void ref() const
    {
        ref(this, d);
    }

    inline void deref() const
    {
        bool v = deref(this, d);
#ifndef NDEBUG
        if (!v) {
            d = 0;
        }
#else
    Q_UNUSED(v);
#endif
    }

private:
    mutable T* d;
};

template<class T>
class KisWeakSharedPtr
{
    friend class KisSharedPtr<T>;
public:
    using element_type = T;

    inline KisWeakSharedPtr()
        : d(0), weakReference(0) { }

    inline KisWeakSharedPtr(T* p) {
        load(p);
    }

    inline KisWeakSharedPtr(const KisSharedPtr<T>& o) {
        load(o.d);
    }

    inline KisWeakSharedPtr(const KisWeakSharedPtr<T>& o) {
        if (o.isConsistent()) {
            load(o.d);
        }
        else {
            d = 0;
            weakReference = 0;
        }
    }

    inline ~KisWeakSharedPtr() {
        detach();
    }

    inline KisWeakSharedPtr& operator= (const KisWeakSharedPtr& o) {
        attach(o);
        return *this;
    }

    inline bool operator== (const T* p) const {
        return (d == p);
    }
    inline bool operator!= (const T* p) const {
        return (d != p);
    }
    inline bool operator== (const KisWeakSharedPtr& o) const {
        return (d == o.d);
    }
    inline bool operator!= (const KisWeakSharedPtr& o) const {
        return (d != o.d);
    }

    inline KisWeakSharedPtr<T>& operator= (T* p) {
        attach(p);
        return *this;
    }

    template< class T2> inline operator KisWeakSharedPtr<T2>() const {
        return KisWeakSharedPtr<T2>(d);
    }

    inline T* data() {
        if (!isConsistent()) {
            warnKrita.noquote() << kisBacktrace();
            Q_ASSERT_X(0, "KisWeakSharedPtr", "Weak pointer is not valid!");
        }

        return d;
    }

    inline const T* data() const {
        if (!isConsistent()) {
            warnKrita.noquote() << kisBacktrace();
            Q_ASSERT_X(0, "KisWeakSharedPtr", "Weak pointer is not valid!");
        }

        return d;
    }

    inline const T* constData() const {
        if (!isConsistent()) {
            warnKrita.noquote() << kisBacktrace();
            Q_ASSERT_X(0, "KisWeakSharedPtr", "Weak pointer is not valid!");
        }

        return d;
    }

    inline operator const T*() const {

        return isConsistent() ? d : 0;
    }

    inline const T& operator*() const {
        if (!isValid()) {
            warnKrita.noquote() << kisBacktrace();
            Q_ASSERT_X(0, "KisWeakSharedPtr", "Weak pointer is not valid!");
        }

        return *d;
    }

    inline T& operator*() {
        if (!isValid()) {
            warnKrita.noquote() << kisBacktrace();
            Q_ASSERT_X(0, "KisWeakSharedPtr", "Weak pointer is not valid!");
        }

        return *d;
    }

    inline const T* operator->() const {
        if (!isValid()) {
            warnKrita.noquote() << kisBacktrace();
            Q_ASSERT_X(0, "KisWeakSharedPtr", "Weak pointer is not valid!");
        }

        return d;
    }

    inline T* operator->() {
        if (!isValid()) {
            warnKrita.noquote() << kisBacktrace();
            Q_ASSERT_X(0, "KisWeakSharedPtr", "Weak pointer is not valid!");
        }

        return d;
    }

    inline bool isNull() const {
        return (d == 0);
    }

    inline bool isValid() const {
        Q_ASSERT(!d || weakReference);

        return d && weakReference && isOdd((int)*weakReference);
    }

    inline KisSharedPtr<T> toStrongRef() const {
        return KisSharedPtr<T>(*this);
    }

private:
    static const qint32 WEAK_REF = 2;
    static inline bool isOdd(const qint32 &x) {
        return x & 0x01;
    }

    inline bool isConsistent() const {
        Q_ASSERT(!d || weakReference);

        return !d || (weakReference && isOdd((int)*weakReference));
    }

    void load(T* newValue) {
        d = newValue;

        if (d) {
            weakReference = d->sharedWeakReference();
            weakReference->fetchAndAddOrdered(WEAK_REF);
        }
        else {
            weakReference = 0;
        }
    }

    inline void attach(T* newValue) {
        detach();
        load(newValue);
    }

    inline void attach(const KisWeakSharedPtr& o) {
        detach();
        if (o.isConsistent()) {
            load(o.d);
        }
        else {
            d = 0;
            weakReference = 0;
        }
    }

    void detach() {
        d = 0;

        if (weakReference &&
            weakReference->fetchAndAddOrdered(-WEAK_REF) <= WEAK_REF) {

            Q_ASSERT((int)*weakReference == 0);

            delete weakReference;
            weakReference = 0;
        }
    }

    mutable T* d;
    QAtomicInt *weakReference;
};

template <class T>
Q_INLINE_TEMPLATE  KisSharedPtr<T>::KisSharedPtr(const KisWeakSharedPtr<T>& o)
        : d(o.d)
{
    if (o.isValid()) {
        ref();

        Q_ASSERT(o.isConsistent());
    }
    else {
        d = 0;
    }
}

template <class T>
Q_INLINE_TEMPLATE void KisSharedPtr<T>::attach(T* p)
{
    if (d != p) {
        ref(this, p);
        T* old = d;
        d = p;
        deref(this, old);
    }
}

template <class T>
Q_INLINE_TEMPLATE void KisSharedPtr<T>::clear()
{
    attach((T*)0);
}

#endif
