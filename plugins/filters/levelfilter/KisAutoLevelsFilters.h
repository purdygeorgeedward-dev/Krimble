/*
 * KRIMBLE 2026-10-02: one-click Auto Tone and Auto Color filters.
 *
 * Both run the existing auto levels engine (KisAutoLevels, the same code
 * behind the "auto levels" buttons in the Levels dialog) with fixed settings
 * and then apply the result through the existing Levels transformation.
 * No new algorithm is introduced.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_AUTO_LEVELS_FILTERS_H
#define KIS_AUTO_LEVELS_FILTERS_H

#include <KoID.h>
#include <klocalizedstring.h>
#include <filter/kis_filter.h>

#include <KisAutoLevels.h>

class KisAutoLevelsFilterBase : public KisFilter
{
public:
    KisAutoLevelsFilterBase(const KoID &id,
                            const QString &menuEntry,
                            KisAutoLevels::MidtonesAdjustmentMethod midtonesMethod,
                            qreal midtonesAmount);

    void processImpl(KisPaintDeviceSP device,
                     const QRect &applyRect,
                     const KisFilterConfigurationSP config,
                     KoUpdater *progressUpdater) const override;

private:
    KisAutoLevels::MidtonesAdjustmentMethod m_midtonesMethod;
    qreal m_midtonesAmount;
};

class KisAutoToneFilter : public KisAutoLevelsFilterBase
{
public:
    KisAutoToneFilter();

    static inline KoID id()
    {
        return KoID("autotone", i18n("Auto Tone"));
    }
};

class KisAutoColorFilter : public KisAutoLevelsFilterBase
{
public:
    KisAutoColorFilter();

    static inline KoID id()
    {
        return KoID("autocolor", i18n("Auto Color"));
    }
};

#endif
