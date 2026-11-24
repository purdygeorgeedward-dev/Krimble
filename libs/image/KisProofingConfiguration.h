/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KISPROOFINGCONFIGURATION_H
#define KISPROOFINGCONFIGURATION_H

#include "KoColor.h"
#include "KoColorConversionTransformation.h"
#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisProofingConfiguration {
public:
    KisProofingConfiguration();
    ~KisProofingConfiguration();
    KoColorConversionTransformation::Intent conversionIntent;
    KoColorConversionTransformation::Intent displayIntent;
    bool useBlackPointCompensationFirstTransform;
    KoColorConversionTransformation::ConversionFlags displayFlags;
    KoColor warningColor;
    QString proofingProfile;
    QString proofingModel;
    QString proofingDepth;

    qreal legacyAdaptationState() const {
        return displayFlags.testFlag(KoColorConversionTransformation::NoAdaptationAbsoluteIntent) ? 0.0 : 1.0;
    }

    void setLegacyAdaptationState(qreal value) {
        displayFlags.setFlag(KoColorConversionTransformation::NoAdaptationAbsoluteIntent, value < 0.5);
    }

    enum DisplayTransformState {
        Monitor = 0,
        Paper,
        Custom
    };
    DisplayTransformState displayMode;

    KoColorConversionTransformation::Intent determineDisplayIntent(KoColorConversionTransformation::Intent monitorDisplayIntent);
    KoColorConversionTransformation::ConversionFlags determineDisplayFlags(KoColorConversionTransformation::ConversionFlags monitorDisplayFlags);

    bool operator==(const KisProofingConfiguration &other) const;
    bool operator!=(const KisProofingConfiguration &other) const;
};

Q_DECLARE_METATYPE(KisProofingConfiguration::DisplayTransformState);

#endif
