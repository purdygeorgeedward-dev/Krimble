/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_GENERATOR_H_
#define _KIS_GENERATOR_H_

#include <QString>

#include <klocalizedstring.h>

#include "KoID.h"
#include "KoColorSpace.h"

#include "kis_types.h"
#include "kis_base_processor.h"
#include "kritaimage_export.h"

class KisProcessingInformation;

class KRITAIMAGE_EXPORT KisGenerator : public KisBaseProcessor
{
    friend class KisGeneratorConfigurationFactory;
public:

    KisGenerator(const KoID& id, const KoID & category, const QString & entry);
    ~KisGenerator() override;

public:

    virtual void generate(KisProcessingInformation dst,
                          const QSize& size,
                          const KisFilterConfigurationSP config,
                          KoUpdater* progressUpdater
                         ) const = 0;

    virtual void generate(KisProcessingInformation dst,
                          const QSize& size,
                          const KisFilterConfigurationSP config
                         ) const;

    virtual QRect generatedRect(QRect _imageArea, const KisFilterConfigurationSP = 0) const;

    virtual bool allowsSplittingIntoPatches() const { return true; }

protected:

    QString configEntryGroup() const;

};

#endif
