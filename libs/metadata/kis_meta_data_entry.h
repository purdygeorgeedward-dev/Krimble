/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KIS_META_DATA_ENTRY_H_
#define _KIS_META_DATA_ENTRY_H_

#include <kritametadata_export.h>
#include <kis_debug.h>

class QString;

namespace KisMetaData
{
class Value;
class Store;
class Schema;
class KRITAMETADATA_EXPORT Entry
{
    struct Private;
    friend class Store;
public:
    Entry();
    Entry(const KisMetaData::Schema* schema, QString name, const KisMetaData::Value& value);
    Entry(const Entry&);
    ~Entry();
    QString name() const;
    const KisMetaData::Schema* schema() const;
    QString qualifiedName() const;
    const KisMetaData::Value& value() const;
    KisMetaData::Value& value();
    bool isValid() const;
    static bool isValidName(const QString& _name);
    Entry& operator=(const Entry& entry);
    bool operator==(const Entry&) const;
private:
    void setSchema(const KisMetaData::Schema* schema);
private:
    Private* const d;
};
}

KRITAMETADATA_EXPORT QDebug operator<<(QDebug debug, const KisMetaData::Entry &c);

#endif
