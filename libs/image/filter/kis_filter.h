/*
 *  SPDX-FileCopyrightText: 2004, 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_FILTER_H_
#define _KIS_FILTER_H_

#include <list>

#include <QString>

#include <klocalizedstring.h>

#include "KoID.h"
#include "KoColorSpace.h"

#include "kis_types.h"
#include "kis_base_processor.h"

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisFilter : public KisBaseProcessor
{
public:

    KisFilter(const KoID& id, const KoID & category, const QString & entry);
    ~KisFilter() override;

    virtual void processImpl(KisPaintDeviceSP device,
                             const QRect& applyRect,
                             const KisFilterConfigurationSP config,
                             KoUpdater* progressUpdater = 0 ) const = 0;

    void process(const KisPaintDeviceSP src,
                 KisPaintDeviceSP dst,
                 KisSelectionSP selection,
                 const QRect& applyRect,
                 const KisFilterConfigurationSP config,
                 KoUpdater* progressUpdater = 0 ) const;

    void process(KisPaintDeviceSP device,
                 const QRect& applyRect,
                 const KisFilterConfigurationSP config,
                 KoUpdater* progressUpdater = 0 ) const;

    virtual QRect neededRect(const QRect & rect, const KisFilterConfigurationSP config, int lod) const;

    virtual QRect changedRect(const QRect & rect, const KisFilterConfigurationSP config, int lod) const;

    virtual bool supportsLevelOfDetail(const KisFilterConfigurationSP config, int lod) const;

    virtual bool needsTransparentPixels(const KisFilterConfigurationSP config, const KoColorSpace *cs) const;

    virtual bool configurationAllowedForMask(KisFilterConfigurationSP config) const;
    virtual void fixLoadedFilterConfigurationForMasks(KisFilterConfigurationSP config) const;

protected:

    QString configEntryGroup() const;
    void setSupportsLevelOfDetail(bool value);

private:
    bool m_supportsLevelOfDetail;
};

#endif
