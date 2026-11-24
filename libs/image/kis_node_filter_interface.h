/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_NODE_FILTER_INTERFACE_H_
#define _KIS_NODE_FILTER_INTERFACE_H_

#include <kritaimage_export.h>
#include <kis_types.h>

class KRITAIMAGE_EXPORT KisNodeFilterInterface
{
public:
    KisNodeFilterInterface(KisFilterConfigurationSP filterConfig);
    KisNodeFilterInterface(const KisNodeFilterInterface &rhs);
    virtual ~KisNodeFilterInterface();

    virtual KisFilterConfigurationSP filter() const;

    virtual void setFilter(KisFilterConfigurationSP filterConfig, bool checkCompareConfig = true);

    virtual void notifyColorSpaceChanged();

private:
    KisNodeFilterInterface& operator=(const KisNodeFilterInterface &other);

    KisFilterConfigurationSP m_filterConfiguration;
};

#endif
