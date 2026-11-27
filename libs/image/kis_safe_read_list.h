/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SAFE_READ_LIST_H_
#define KIS_SAFE_READ_LIST_H_

#include <QList>

template<class T> class KisSafeReadList : private QList<T> {
public:
    KisSafeReadList() {}

    using typename QList<T>::const_iterator;

    inline const T& first() const {
        return QList<T>::first();
    }

    inline const T& last() const {
        return QList<T>::last();
    }

    inline const T& at(int i) const {
        return QList<T>::at(i);
    }

    using QList<T>::constBegin;
    using QList<T>::constEnd;
    using QList<T>::isEmpty;
    using QList<T>::size;
    using QList<T>::indexOf;
    using QList<T>::contains;

    using QList<T>::append;
    using QList<T>::prepend;
    using QList<T>::insert;
    using QList<T>::removeAt;
    using QList<T>::clear;

private:
    Q_DISABLE_COPY(KisSafeReadList)
};

#define FOREACH_SAFE(_iter, _container)         \
    for(_iter = _container.constBegin();        \
        _iter != _container.constEnd();         \
        _iter++)

#endif
