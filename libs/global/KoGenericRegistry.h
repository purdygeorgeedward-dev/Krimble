/* This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2004 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_GENERIC_REGISTRY_H_
#define _KO_GENERIC_REGISTRY_H_

#include <QList>
#include <QString>
#include <QHash>

#include "kis_assert.h"

template<typename T>
class KoGenericRegistry
{
public:
    KoGenericRegistry() { }
    virtual ~KoGenericRegistry()
    {
        m_doubleEntries.clear();
        m_hash.clear();
    }

public:
    void add(T item)
    {
        KIS_SAFE_ASSERT_RECOVER_RETURN(item);

        const QString id = item->id();
        KIS_SAFE_ASSERT_RECOVER_NOOP(!m_aliases.contains(id));

        if (m_hash.contains(id)) {
            m_doubleEntries << value(id);
            remove(id);
        }
        m_hash.insert(id, item);
    }

    void add(const QString &id, T item)
    {
        KIS_SAFE_ASSERT_RECOVER_RETURN(item);
        KIS_SAFE_ASSERT_RECOVER_NOOP(!m_aliases.contains(id));

        if (m_hash.contains(id)) {
            m_doubleEntries << value(id);
            remove(id);
        }
        m_hash.insert(id, item);
    }

    void remove(const QString &id)
    {
        m_hash.remove(id);
    }

    void addAlias(const QString &alias, const QString &id)
    {
        KIS_SAFE_ASSERT_RECOVER_NOOP(!m_hash.contains(alias));
        m_aliases[alias] = id;
    }

    void removeAlias(const QString &alias)
    {
        m_aliases.remove(alias);
    }

    T get(const QString &id) const
    {
        return value(id);
    }

    bool contains(const QString &id) const
    {
        bool result = m_hash.contains(id);

        if (!result && m_aliases.contains(id)) {
            result = m_hash.contains(m_aliases.value(id));
        }

        return result;
    }

    const T value(const QString &id) const
    {
        T result = m_hash.value(id);

        if (!result && m_aliases.contains(id)) {
            result = m_hash.value(m_aliases.value(id));
        }

        return result;
    }

    QList<QString> keys() const
    {
        return m_hash.keys();
    }

    int count() const
    {
        return m_hash.count();
    }

    QList<T> values() const
    {
        return m_hash.values();
    }

    QList<T> doubleEntries() const
    {
        return m_doubleEntries;
    }

    typename QHash<QString, T>::const_iterator constBegin() const {
        return m_hash.constBegin();
    }

    typename QHash<QString, T>::const_iterator constEnd() const {
        return m_hash.constEnd();
    }

private:

    QList<T> m_doubleEntries;

private:

    QHash<QString, T> m_hash;
    QHash<QString, QString> m_aliases;
};

#endif
