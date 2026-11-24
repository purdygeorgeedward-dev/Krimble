/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KIS_META_DATA_SCHEMA_REGISTRY_H_
#define _KIS_META_DATA_SCHEMA_REGISTRY_H_

#include <kritametadata_export.h>

class QString;
class QDebug;

namespace KisMetaData
{

class Schema;

class KRITAMETADATA_EXPORT SchemaRegistry
{
    struct Private;
public:
    SchemaRegistry();
    ~SchemaRegistry();
public:
    const KisMetaData::Schema* create(const QString & uri, const QString & prefix);
    const Schema* schemaFromUri(const QString & uri) const;
    const Schema* schemaFromPrefix(const QString & prefix) const;
    static KisMetaData::SchemaRegistry* instance();
private:
    Private* const d;
};
}

KRITAMETADATA_EXPORT QDebug operator<<(QDebug debug, const KisMetaData::Schema &c);

#endif
