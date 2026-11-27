/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KIS_META_DATA_MERGE_STRATEGY_H_
#define _KIS_META_DATA_MERGE_STRATEGY_H_

#include <QList>

#include <kritametadata_export.h>

class QString;

namespace KisMetaData
{
class Store;
class KRITAMETADATA_EXPORT MergeStrategy
{
public:
    virtual ~MergeStrategy();
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual void merge(Store* dst, QList<const Store*> srcs, QList<double> scores) const = 0;
};

}

#endif
