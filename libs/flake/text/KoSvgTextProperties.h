/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOSVGTEXTPROPERTIES_H
#define KOSVGTEXTPROPERTIES_H

#include "kritaflake_export.h"
#include "KoFlakeTypes.h"
#include "KoSvgText.h"

#include <QScopedPointer>
#include <QVariant>
#include <QList>

#include <boost/operators.hpp>

#include "KoCSSFontInfo.h"

class SvgLoadingContext;
class KoShapeBackground;

class KRITAFLAKE_EXPORT KoSvgTextProperties : public boost::equality_comparable<KoSvgTextProperties>
{
public:
    enum PropertyId {
        WritingModeId,
        DirectionId,
        UnicodeBidiId,
        TextAnchorId,
        DominantBaselineId,
        AlignmentBaselineId,
        BaselineShiftModeId,
        BaselineShiftValueId,
        KerningId,
        TextOrientationId,
        LetterSpacingId,
        WordSpacingId,

        FontFamiliesId,
        FontStyleId,
        FontStretchId,
        FontWeightId,
        FontSizeId,
        FontSizeAdjustId,

        FontVariantLigatureId,
        FontVariantPositionId,
        FontVariantCapsId,
        FontVariantNumericId,
        FontVariantEastAsianId,

        FontFeatureSettingsId,
        FontOpticalSizingId,
        FontVariationSettingsId,

        TextDecorationLineId,
        TextDecorationStyleId,
        TextDecorationColorId,
        TextDecorationPositionId,
        FillId,
        StrokeId,
        Opacity,
        PaintOrder,
        Visibility,

        TextLanguage,

        TextCollapseId,
        TextWrapId,
        TextTrimId,
        LineBreakId,
        WordBreakId,
        TextAlignAllId,
        TextAlignLastId,
        TextTransformId,
        TextOverFlowId,
        OverflowWrapId,
        InlineSizeId,
        LineHeightId,
        TextIndentId,
        HangingPunctuationId,
        TabSizeId,

        ShapePaddingId,
        ShapeMarginId,

        FontSynthesisBoldId,
        FontSynthesisItalicId,
        FontSynthesisSmallCapsId,
        FontSynthesisSuperSubId,

        TextRenderingId,

        KraTextVersionId,
        KraTextStyleType,
        KraTextStyleResolution,

        LastPropertyId
    };

    KoSvgTextProperties();
    ~KoSvgTextProperties();

    KoSvgTextProperties(const KoSvgTextProperties &rhs);
    KoSvgTextProperties& operator=(const KoSvgTextProperties &rhs);
    bool operator==(const KoSvgTextProperties &rhs) const;

    void setProperty(PropertyId id, const QVariant &value);

    bool hasProperty(PropertyId id) const;

    QVariant property(PropertyId id, const QVariant &defaultValue = QVariant()) const;

    void removeProperty(PropertyId id);

    QVariant propertyOrDefault(PropertyId id) const;

    QList<PropertyId> properties() const;

    bool isEmpty() const;

    void resetNonInheritableToDefault();

    void inheritFrom(const KoSvgTextProperties &parentProperties, bool resolve = false, bool onlyFontAndLineHeight = false);

    void resolveRelativeValues(const KoSvgText::FontMetrics metrics = KoSvgText::FontMetrics(12.0, true), const qreal fontSize = 12.0, bool onlyFontAndLineHeight = false);

    bool inheritsProperty(PropertyId id, const KoSvgTextProperties &parentProperties) const;

    bool hasNonInheritableProperties() const;

    void setAllButNonInheritableProperties(const KoSvgTextProperties &properties);

    void scaleAbsoluteValues(const double scaleInline = 1.0, const double scaleBlock = 1.0);

    KoSvgTextProperties ownProperties(const KoSvgTextProperties &parentProperties, bool keepFontSize = false) const;

    void parseSvgTextAttribute(const SvgLoadingContext &context, const QString &command, const QString &value);

    QMap<QString, QString> convertToSvgTextAttributes() const;

    QMap<QString, QString> convertParagraphProperties() const;

    QFont generateFont() const;

    qreal xHeight() const;

    KoSvgText::FontMetrics metrics(const bool withResolvedLineHeight = true, const bool offsetByBaseline = false) const;

    KoSvgText::FontMetrics applyLineHeight(KoSvgText::FontMetrics metrics) const;

    QStringList fontFeaturesForText(int start, int length) const;

    KoCSSFontInfo cssFontInfo() const;

    QSharedPointer<KoShapeBackground> background() const;
    KoShapeStrokeModelSP stroke() const;

    KoSvgText::CssLengthPercentage fontSize() const;
    void setFontSize(const KoSvgText::CssLengthPercentage length);

    static QStringList supportedXmlAttributes();

    static const KoSvgTextProperties& defaultProperties();

    static bool propertyIsBlockOnly(KoSvgTextProperties::PropertyId id);

    bool propertyIsInheritable(KoSvgTextProperties::PropertyId id) const;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
