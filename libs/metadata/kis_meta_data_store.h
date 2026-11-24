/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KIS_META_DATA_STORE_H_
#define _KIS_META_DATA_STORE_H_

#include <kritametadata_export.h>

#include <QHash>

namespace KisMetaData
{
class Schema;
class Entry;
class Filter;
class Value;

class KRITAMETADATA_EXPORT Store
{
    struct Private;
public:
    Store();
    Store(const Store&);
    ~Store();
public:

    void copyFrom(const Store* store);

    bool empty() const;
    bool isEmpty() const;

    bool addEntry(const Entry& entry);

    Entry& getEntry(const QString & entryKey);

    Entry& getEntry(const QString & uri, const QString & entryName);

    Entry& getEntry(const KisMetaData::Schema* schema, const QString & entryName);

    const Entry& getEntry(const QString & entryKey) const;
    const Entry& getEntry(const QString & uri, const QString & entryName) const;

    const Entry& getEntry(const KisMetaData::Schema* schema, const QString & entryName) const;

    void removeEntry(const QString & entryKey);

    void removeEntry(const QString & uri, const QString & entryName);

    void removeEntry(const KisMetaData::Schema* schema, const QString & entryName);

    const Value& getValue(const QString & uri, const QString & entryName) const;

    QHash<QString, Entry>::const_iterator begin() const;
    QHash<QString, Entry>::const_iterator end() const;

    bool containsEntry(const QString & entryKey) const;

    bool containsEntry(const KisMetaData::Schema* schema, const QString & entryName) const;

    bool containsEntry(const QString & uri, const QString & entryName) const;

    void debugDump() const;

    void applyFilters(const QList<const Filter*> & filters);

    QList<QString> keys() const;

    QList<Entry> entries() const;
private:
    Private* const d;
};
}

#endif
