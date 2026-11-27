/*
 *  SPDX-FileCopyrightText: 2007-2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KIS_META_DATA_FILTER_H_
#define _KIS_META_DATA_FILTER_H_

#include <kritametadata_export.h>
class QString;

namespace KisMetaData
{
class Store;
class Filter
{
public:
    virtual ~Filter();
    virtual bool defaultEnabled() const = 0;
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual void filter(KisMetaData::Store*) const = 0;
};
}

#endif
