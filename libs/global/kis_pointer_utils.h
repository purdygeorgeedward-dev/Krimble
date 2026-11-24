/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_POINTER_UTILS_H
#define KIS_POINTER_UTILS_H

#include <QSharedPointer>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QForeach>
#endif

template <class T>
inline QSharedPointer<T> toQShared(T* ptr) {
    return QSharedPointer<T>(ptr);
}

template <class A, template <class C> class List>
List<QSharedPointer<A>> listToQShared(const List<A*> list) {
    List<QSharedPointer<A>> newList;
    Q_FOREACH(A* value, list) {
        newList.append(toQShared(value));
    }
    return newList;
}

template <template <class> class Container, class T>
Container<QWeakPointer<T>> listStrongToWeak(const Container<QSharedPointer<T>> &container)
{
    Container<QWeakPointer<T> > result;
    Q_FOREACH (QSharedPointer<T> v, container) {
        result << v;
    }
    return result;
}

template <template <class> class Container, class T>
    Container<QSharedPointer<T> > listWeakToStrong(const Container<QWeakPointer<T>> &container,
                                                   bool allOrNothing = true)
{
    Container<QSharedPointer<T> > result;
    Q_FOREACH (QWeakPointer<T> v, container) {
        QSharedPointer<T> strong(v);
        if (!strong && allOrNothing) {
            result.clear();
            return result;
        }

        if (strong) {
            result << strong;
        }
    }
    return result;
}

template <typename R, typename T, template <typename U> class Container>
inline Container<R> implicitCastList(const Container<T> &list)
{
    Container<R> result;

    Q_FOREACH(const T &item, list) {
        result.append(item);
    }
    return result;
}

template<class T>
class KisWeakSharedPtr;
template<class T>
class KisSharedPtr;
template<class T>
class KisPinnedSharedPtr;

template <typename T>
T* removeSharedPointer(T* value)
{
    return value;
}

template <typename T>
T* removeSharedPointer(KisPinnedSharedPtr<T> value)
{
    return value.data();
}

template <typename T>
T* removeSharedPointer(KisSharedPtr<T> value)
{
    return value.data();
}

template <typename T>
T* removeSharedPointer(QSharedPointer<T> value)
{
    return value.data();
}

template <typename T>
struct KisSharedPointerTraits
{
};

template <typename T>
struct KisSharedPointerTraits<QSharedPointer<T>>
{
    template <typename U>
    using SharedPointerType = QSharedPointer<U>;
    using ValueType = T;

    template <typename D, typename S>
    static inline QSharedPointer<D> dynamicCastSP(QSharedPointer<S> src) {
        return src.template dynamicCast<D>();
    }
};

template <typename T>
struct KisSharedPointerTraits<KisSharedPtr<T>>
{
    template <typename U>
    using SharedPointerType = KisSharedPtr<U>;
    using ValueType = T;

    template <typename D, typename S>
    static inline KisSharedPtr<D> dynamicCastSP(KisSharedPtr<S> src) {
        return KisSharedPtr<D>(dynamic_cast<D*>(src.data()));
    }
};

template <typename T>
struct KisSharedPointerTraits<KisPinnedSharedPtr<T>>
{
    template <typename U>
    using SharedPointerType = KisPinnedSharedPtr<U>;
    using ValueType = T;

    template <typename D, typename S>
    static inline KisPinnedSharedPtr<D> dynamicCastSP(KisPinnedSharedPtr<S> src) {
        return KisPinnedSharedPtr<D>(dynamic_cast<D*>(src.data()));
    }
};

#endif

