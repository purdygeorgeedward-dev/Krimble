/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOSVGTEXT_H
#define KOSVGTEXT_H

#include <QFont>
#include <QPainterPath>
#include <QTextCharFormat>
#include <QVariant>
#include <QLocale>
#include <array>
#include <boost/operators.hpp>
#include <boost/optional.hpp>

#include <QSharedPointer>
#include <KoShapeBackground.h>
#include <KoShapeStrokeModel.h>

#include <QDomDocument>

#include <kritaflake_export.h>

class SvgLoadingContext;
class QDebug;

#include <KoShape.h>
class KoSvgTextChunkShape;

namespace KoSvgText
{
    Q_NAMESPACE_EXPORT(KRITAFLAKE_EXPORT)

enum WritingMode {
    HorizontalTB,
    VerticalRL,
    VerticalLR
};
Q_ENUM_NS(WritingMode)

enum Direction {
    DirectionLeftToRight,
    DirectionRightToLeft
};
Q_ENUM_NS(Direction)

enum UnicodeBidi {
    BidiNormal,
    BidiEmbed,
    BidiOverride,
    BidiIsolate,
    BidiIsolateOverride,
    BidiPlainText
};
Q_ENUM_NS(UnicodeBidi)

enum TextOrientation {
    OrientationMixed,
    OrientationUpright,
    OrientationSideWays
};
Q_ENUM_NS(TextOrientation)

enum TextAnchor {
    AnchorStart,
    AnchorMiddle,
    AnchorEnd
};
Q_ENUM_NS(TextAnchor)

enum TextSpaceCollapse {
    Collapse,
    Discard,
    Preserve,
    PreserveBreaks,
    PreserveSpaces,
    BreakSpaces
};
Q_ENUM_NS(TextSpaceCollapse)

enum TextWrap {
    Wrap,
    NoWrap,
    Balance,
    Stable,
    Pretty
};
Q_ENUM_NS(TextWrap)

enum TextSpaceTrim {
    TrimNone = 0x0,
    TrimInner = 0x1,
    DiscardBefore = 0x2,
    DiscardAfter = 0x4
};
Q_ENUM_NS(TextSpaceTrim)

enum WordBreak {
    WordBreakNormal,
    WordBreakKeepAll,
    WordBreakBreakAll,
};
Q_ENUM_NS(WordBreak)

enum LineBreak {
    LineBreakAuto,
    LineBreakLoose,
    LineBreakNormal,
    LineBreakStrict,
    LineBreakAnywhere
};
Q_ENUM_NS(LineBreak)

enum OverflowWrap {
    OverflowWrapNormal,
    OverflowWrapAnywhere,
    OverflowWrapBreakWord
};

enum TextAlign {
    AlignLastAuto,
    AlignStart,
    AlignEnd,
    AlignLeft,
    AlignRight,
    AlignCenter,
    AlignJustify,
    AlignMatchParent
};
Q_ENUM_NS(TextAlign)

enum TextTransform {
    TextTransformNone = 0x0,
    TextTransformCapitalize = 0x1,
    TextTransformUppercase = 0x2,
    TextTransformLowercase = 0x4,
};
Q_ENUM_NS(TextTransform)

enum TextOverflow {
    OverFlowVisible,
    OverFlowClip,
    OverFlowEllipse
};

enum HangingPunctuation {
    HangNone = 0x0,
    HangFirst = 0x1,
    HangLast = 0x2,
    HangEnd = 0x4,
    HangForce = 0x8
};

enum Baseline {
    BaselineAuto,
    BaselineUseScript,
    BaselineDominant,
    BaselineNoChange,
    BaselineResetSize,
    BaselineIdeographic,
    BaselineAlphabetic,
    BaselineHanging,
    BaselineMathematical,
    BaselineCentral,
    BaselineMiddle,
    BaselineTextBottom,
    BaselineTextTop
};
Q_ENUM_NS(Baseline)

enum BaselineShiftMode {
    ShiftNone,
    ShiftSub,
    ShiftSuper,
    ShiftLengthPercentage,
    ShiftLineTop,
    ShiftLineBottom
};
Q_ENUM_NS(BaselineShiftMode)

enum LengthAdjust {
    LengthAdjustSpacing,
    LengthAdjustSpacingAndGlyphs
};
Q_ENUM_NS(LengthAdjust)

enum TextDecoration {
    DecorationNone = 0x0,
    DecorationUnderline = 0x1,
    DecorationOverline = 0x2,
    DecorationLineThrough = 0x4
};

enum TextDecorationStyle {
    Solid,
    Double,
    Dotted,
    Dashed,
    Wavy
};
Q_ENUM_NS(TextDecorationStyle)

enum TextDecorationUnderlinePosition {
    UnderlineAuto,
    UnderlineUnder,
    UnderlineLeft,
    UnderlineRight
};
Q_ENUM_NS(TextDecorationUnderlinePosition)

enum TextPathMethod {
    TextPathAlign,
    TextPathStretch
};

enum TextPathSpacing {
    TextPathAuto,
    TextPathExact
};

enum TextPathSide {
    TextPathSideRight,
    TextPathSideLeft
};

Q_DECLARE_FLAGS(TextDecorations, TextDecoration)
Q_DECLARE_OPERATORS_FOR_FLAGS(TextDecorations)

Q_DECLARE_FLAGS(TextSpaceTrims, TextSpaceTrim)
Q_DECLARE_OPERATORS_FOR_FLAGS(TextSpaceTrims)

Q_DECLARE_FLAGS(HangingPunctuations, HangingPunctuation)
Q_DECLARE_OPERATORS_FOR_FLAGS(HangingPunctuations)

enum TextRendering {
    RenderingAuto,
    RenderingOptimizeSpeed,
    RenderingOptimizeLegibility,
    RenderingGeometricPrecision
};
Q_ENUM_NS(TextRendering)

struct FontMetrics : public boost::equality_comparable<FontMetrics> {
    bool isVertical = false;
    qint32 fontSize = 0;
    qint32 zeroAdvance = 0;
    qint32 spaceAdvance = 0;
    qint32 ideographicAdvance = 0;

    qint32 xHeight = 0;
    qint32 capHeight = 0;
    QPair<qint32, qint32> subScriptOffset = qMakePair(0, 0);
    QPair<qint32, qint32> superScriptOffset = qMakePair(0, 0);

    qint32 ascender = 0;
    qint32 descender = 0;
    qint32 lineGap = 0;

    qint32 alphabeticBaseline = 0;
    qint32 mathematicalBaseline = 0;

    qint32 ideographicUnderBaseline = 0;
    qint32 ideographicCenterBaseline = 0;
    qint32 ideographicOverBaseline = 0;

    qint32 ideographicFaceUnderBaseline = 0;
    qint32 ideographicFaceOverBaseline = 0;

    qint32 hangingBaseline = 0;

    qint32 underlineOffset = 0;
    qint32 underlineThickness = 0;
    qint32 lineThroughOffset = 0;
    qint32 lineThroughThickness = 0;

    qint32 caretRun = 0;
    qint32 caretRise = 0;
    qint32 caretOffset = 0;

    FontMetrics () {

    }

    FontMetrics (qreal fontSizeInPt, bool isHorizontal);

    bool operator==(const FontMetrics & other) const;

    int valueForBaselineValue(Baseline baseline) const;

    void setBaselineValueByTag(const QString &tag, int32_t value);

    void setMetricsValueByTag(const QLatin1String &tag, int32_t value);

    void scaleBaselines(const qreal multiplier);

    void offsetMetricsToNewOrigin(const Baseline baseline);
};
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::FontMetrics &metrics);

struct CssLengthPercentage : public boost::equality_comparable<CssLengthPercentage> {
    enum UnitType {
        Absolute,
        Percentage,
        Em,
        Ex,
        Cap,
        Ch,
        Ic,
        Lh,
    };

    CssLengthPercentage() {}
    CssLengthPercentage(qreal value, UnitType unit = Absolute): value(value), unit(unit) {}

    qreal value = 0.0;
    UnitType unit = Absolute;

    void convertToAbsolute(const KoSvgText::FontMetrics metrics, const qreal fontSize, const UnitType percentageUnit = Em);

    bool operator==(const CssLengthPercentage & other) const {
        return qFuzzyCompare(value, other.value) && unit == other.unit;
    }
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::CssLengthPercentage &value);

QString writeLengthPercentage(const CssLengthPercentage &length, bool percentageAsEm = false);

struct AutoValue : public boost::equality_comparable<AutoValue>
{
    AutoValue() {}
    AutoValue(qreal _customValue) : isAuto(false), customValue(_customValue) {}

    bool isAuto = true;
    qreal customValue = 0.0;

    bool operator==(const AutoValue & other) const {
        return isAuto == other.isAuto && (isAuto || qFuzzyCompare(customValue, other.customValue));
    }
};

struct AutoLengthPercentage : public boost::equality_comparable<AutoLengthPercentage>
{
    AutoLengthPercentage() {}
    AutoLengthPercentage(CssLengthPercentage _length)
        : isAuto(false), length(_length) {}
    AutoLengthPercentage(qreal _customValue, CssLengthPercentage::UnitType unit = CssLengthPercentage::Absolute)
        : isAuto(false), length(_customValue, unit) {}

    bool isAuto = true;
    CssLengthPercentage length;

    bool operator==(const AutoLengthPercentage & other) const {
        return isAuto == other.isAuto && (isAuto || length == other.length);
    }
};

struct CssFontStyleData : public boost::equality_comparable<CssFontStyleData>
{
    CssFontStyleData() {}
    CssFontStyleData(QFont::Style _style): style(_style){}
    QFont::Style style = QFont::StyleNormal;
    KoSvgText::AutoValue slantValue;

    bool operator==(const CssFontStyleData & other) const {
        return style == other.style
                && (style != QFont::StyleOblique || slantValue == other.slantValue);
    }
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::AutoValue &value);
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::AutoLengthPercentage &value);
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::CssFontStyleData &value);

inline QVariant fromAutoValue(const KoSvgText::AutoValue &value) {
    return QVariant::fromValue(value);
}

AutoValue parseAutoValueX(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword = "auto");
AutoValue parseAutoValueY(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword = "auto");
AutoValue parseAutoValueXY(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword = "auto");
AutoValue parseAutoValueAngular(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword = "auto");

AutoLengthPercentage parseAutoLengthPercentageXY(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword = "auto", QRectF bbox = QRectF(), bool percentageIsViewPort = true);

WritingMode parseWritingMode(const QString &value);
Direction parseDirection(const QString &value);
UnicodeBidi parseUnicodeBidi(const QString &value);
TextOrientation parseTextOrientation(const QString &value);
TextOrientation parseTextOrientationFromGlyphOrientation(AutoValue value);
TextAnchor parseTextAnchor(const QString &value);

TextRendering parseTextRendering(const QString &value);

bool whiteSpaceValueToLongHands(const QString &value, TextSpaceCollapse &collapseMethod, TextWrap &wrapMethod, TextSpaceTrims &trimMethod);
bool xmlSpaceToLongHands(const QString &value, TextSpaceCollapse &collapseMethod);

WordBreak parseWordBreak(const QString &value);
LineBreak parseLineBreak(const QString &value);
TextAlign parseTextAlign(const QString &value);

CssFontStyleData parseFontStyle(const QString &value);

Baseline parseBaseline(const QString &value);
BaselineShiftMode parseBaselineShiftMode(const QString &value);

LengthAdjust parseLengthAdjust(const QString &value);

static const std::array<const char *, 9> fontStretchNames = {"ultra-condensed",
                                                             "extra-condensed",
                                                             "condensed",
                                                             "semi-condensed",
                                                             "normal",
                                                             "semi-expanded",
                                                             "expanded",
                                                             "extra-expanded",
                                                             "ultra-expanded"};
static const std::array<const char *, 7> fontSizeNames =
    {"xx-small", "x-small", "small", "medium", "large", "x-large", "xx-large"};
int parseCSSFontStretch(const QString &value, int currentStretch);

int parseCSSFontWeight(const QString &value, int currentWeight);

TextPathMethod parseTextPathMethod(const QString &value);
TextPathSpacing parseTextPathSpacing(const QString &value);
TextPathSide parseTextPathSide(const QString &value);

QString writeAutoValue(const AutoValue &value, const QString &autoKeyword = "auto");

QString writeAutoLengthPercentage(const AutoLengthPercentage &value, const QString &autoKeyword = "auto", bool percentageToEm = false);

QString writeWritingMode(WritingMode value, bool svg1_1 = false);
QString writeDirection(Direction value);
QString writeUnicodeBidi(UnicodeBidi value);
QString writeTextOrientation(TextOrientation orientation);
QString writeTextAnchor(TextAnchor value);
QString writeDominantBaseline(Baseline value);
QString writeAlignmentBaseline(Baseline value);
QString writeBaselineShiftMode(BaselineShiftMode value, CssLengthPercentage shift);
QString writeLengthAdjust(LengthAdjust value);

QString writeTextPathMethod(TextPathMethod value);
QString writeTextPathSpacing(TextPathSpacing value);
QString writeTextPathSide(TextPathSide value);

QString writeWordBreak(WordBreak value);
QString writeLineBreak(LineBreak value);
QString writeTextAlign(TextAlign value);

QString writeFontStyle(CssFontStyleData value);

QString writeTextRendering(TextRendering value);

QString writeWhiteSpaceValue(TextSpaceCollapse collapseMethod, TextWrap wrapMethod, KoSvgText::TextSpaceTrims trimMethod);
QString writeXmlSpace(TextSpaceCollapse collapseMethod);

struct CharTransformation : public boost::equality_comparable<CharTransformation>
{
    boost::optional<qreal> xPos;
    boost::optional<qreal> yPos;
    boost::optional<qreal> dxPos;
    boost::optional<qreal> dyPos;
    boost::optional<qreal> rotate;

    void mergeInParentTransformation(const CharTransformation &t);
    bool isNull() const;
    bool startsNewChunk() const;
    bool hasRelativeOffset() const;

    QPointF absolutePos() const;
    QPointF relativeOffset() const;

    bool operator==(const CharTransformation & other) const;
};
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::CharTransformation &t);

struct TextOnPathInfo {
    qreal startOffset = 0.0;
    bool startOffsetIsPercentage = false;
    TextPathMethod method = TextPathAlign;
    TextPathSpacing spacing = TextPathAuto;
    TextPathSide side = TextPathSideLeft;
};

struct TextTransformInfo : public boost::equality_comparable<TextTransformInfo> {
    TextTransformInfo() = default;
    TextTransform capitals = TextTransformNone;
    bool fullWidth = false;
    bool fullSizeKana = false;
    bool operator==(const TextTransformInfo &rhs) const
    {
        return (capitals == rhs.capitals) && (fullWidth == rhs.fullWidth) && (fullSizeKana == rhs.fullSizeKana);
    }
};
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::TextTransformInfo &t);
TextTransformInfo parseTextTransform(const QString &value);
QString writeTextTransform(TextTransformInfo textTransform);

struct TextIndentInfo : public boost::equality_comparable<TextIndentInfo> {
    TextIndentInfo() = default;

    CssLengthPercentage length;
    bool hanging = false;
    bool eachLine = false;
    bool operator==(const TextIndentInfo &rhs) const
    {
        return (length == rhs.length) && (hanging == rhs.hanging) && (eachLine == rhs.eachLine);
    }
};

TextIndentInfo parseTextIndent(const QString &value, const SvgLoadingContext &context);
QString writeTextIndent(TextIndentInfo textIndent);

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::TextIndentInfo &value);

struct TabSizeInfo : public boost::equality_comparable<TabSizeInfo> {
    qreal value = 8;
    bool isNumber = true;
    CssLengthPercentage length;
    qreal extraSpacing = 0.0;
    bool operator==(const TabSizeInfo &rhs) const
    {
        bool val = isNumber? qFuzzyCompare(value, rhs.value): length == rhs.length;
        return (val) && (isNumber == rhs.isNumber);
    }
};
TabSizeInfo parseTabSize(const QString &value, const SvgLoadingContext &context);
QString writeTabSize(TabSizeInfo tabSize);
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::TabSizeInfo &value);

struct LineHeightInfo : public boost::equality_comparable<LineHeightInfo> {
    CssLengthPercentage length;
    qreal value = 1.0;
    bool isNumber = false;
    bool isNormal = true;

    bool operator==(const LineHeightInfo &rhs) const
    {
        bool toggles = (isNumber == rhs.isNumber && isNormal == rhs.isNormal);
        bool val = isNumber? qFuzzyCompare(value, rhs.value): length == rhs.length;
        return (toggles && val);
    }
};

LineHeightInfo parseLineHeight(const QString &value, const SvgLoadingContext &context);
QString writeLineHeight(LineHeightInfo lineHeight);
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::LineHeightInfo &value);

struct BackgroundProperty : public boost::equality_comparable<BackgroundProperty>
{
    BackgroundProperty() {}
    BackgroundProperty(QSharedPointer<KoShapeBackground> p) : property(p) {}

    bool operator==(const BackgroundProperty &rhs) const {
        return (!property && !rhs.property) ||
                (property && rhs.property &&
                 property->compareTo(rhs.property.data()));
    }

    QSharedPointer<KoShapeBackground> property;
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::BackgroundProperty &prop);

struct StrokeProperty : public boost::equality_comparable<StrokeProperty>
{
    StrokeProperty() {}
    StrokeProperty(QSharedPointer<KoShapeStrokeModel> p) : property(p) {}

    bool operator==(const StrokeProperty &rhs) const {
        return (!property && !rhs.property) ||
                (property && rhs.property &&
                 property->compareFillTo(rhs.property.data()) && property->compareStyleTo(rhs.property.data()));
    }

    QSharedPointer<KoShapeStrokeModel> property;
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::StrokeProperty &prop);

struct FontFamilyAxis : public boost::equality_comparable<FontFamilyAxis> {

    FontFamilyAxis() {}
    FontFamilyAxis (QString _tag, qreal _value)
        : tag(_tag), value(_value) {}

    static FontFamilyAxis weightAxis(qreal val) {
        FontFamilyAxis axis("wght", val);
        axis.min = val;
        axis.max = val;
        axis.defaultValue = 400;
        return axis;
    }
    static FontFamilyAxis widthAxis(qreal val) {
        FontFamilyAxis axis("wdth", val);
        axis.min = val;
        axis.max = val;
        axis.defaultValue = 100;
        return axis;
    }
    static FontFamilyAxis slantAxis(qreal val) {
        FontFamilyAxis axis("slnt", val);
        axis.min = val;
        axis.max = val;
        axis.defaultValue = 0;
        return axis;
    }

    QString tag;
    QHash<QLocale, QString> localizedLabels;
    qreal min = -1;
    qreal max = -1;
    qreal value = 0;
    qreal defaultValue = 0;
    bool variableAxis = false;
    bool axisHidden = false;

    QString debugInfo() const {
        QString label;
        if (!localizedLabels.isEmpty()) {
            label = localizedLabels.value(QLocale(QLocale::English), localizedLabels.values().first());
        }
        return QString("Axis: %1 (%2), min: %3, default:%4, max: %5").arg(tag).arg(label).arg(min).arg(value).arg(max);
    }

    bool operator==(const FontFamilyAxis & other) const {
        return (other.tag != tag)
                && (!qFuzzyCompare(other.min, min))
                && (!qFuzzyCompare(other.max, max))
                && (!qFuzzyCompare(other.defaultValue, defaultValue))
                && (!qFuzzyCompare(other.value, value));
    }
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::FontFamilyAxis &axis);
QDataStream KRITAFLAKE_EXPORT &operator<<(QDataStream &out, const KoSvgText::FontFamilyAxis &axis);
QDataStream KRITAFLAKE_EXPORT &operator>>(QDataStream &in, KoSvgText::FontFamilyAxis &axis);

enum FontFormatType {
    UnknownFontType,
    BDFFontType,
    Type1FontType,
    OpenTypeFontType
};
Q_ENUM_NS(FontFormatType)

struct FontFamilyStyleInfo : public boost::equality_comparable<FontFamilyStyleInfo> {
    QHash<QLocale, QString> localizedLabels;
    QHash<QString, float> instanceCoords;

    bool isItalic = false;
    bool isOblique = false;

    QString debugInfo() const {
        QString label;
        if (!localizedLabels.isEmpty()) {
            label = localizedLabels.value(QLocale(QLocale::English), localizedLabels.values().first());
        }
        QStringList coords;
        for (int i = 0; i < instanceCoords.size(); i++) {
            QString key = instanceCoords.keys().at(i);
            coords.append(key+"="+QString::number(instanceCoords.value(key)));
        }
        return QString("Instance: %1, coords: [ %2 ]").arg(label).arg(coords.join(" "));
    }

    bool operator==(const FontFamilyStyleInfo & other) const {
        return (other.instanceCoords != instanceCoords)
                && (other.isItalic != isItalic)
                && (other.isOblique != isOblique);
    }
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::FontFamilyStyleInfo &style);

struct FontFeatureLigatures : public boost::equality_comparable<FontFeatureLigatures> {
    bool commonLigatures = true;
    bool discretionaryLigatures = false;
    bool historicalLigatures = false;
    bool contextualAlternates = true;

    bool operator==(const FontFeatureLigatures & other) const {
        return (other.commonLigatures == commonLigatures
                && other.discretionaryLigatures == discretionaryLigatures
                && other.historicalLigatures == historicalLigatures
                && other.contextualAlternates == contextualAlternates);
    }

    QStringList fontFeatures(const int start, const int end) {
        QStringList list;
        const QString length = QString("[%1:%2]").arg(start).arg(end);
        if (!commonLigatures) {
            list << "clig" + length + "=0";
            list << "liga" + length + "=0";
        }
        if (discretionaryLigatures) {
            list << "dlig" + length + "=1";
        }
        if (historicalLigatures) {
            list << "hlig" + length + "=1";
        }
        if (!contextualAlternates) {
            list << "calt" + length + "=0";
        }
        return list;
    }
};
FontFeatureLigatures parseFontFeatureLigatures(const QString &value, FontFeatureLigatures features);
QString writeFontFeatureLigatures(const FontFeatureLigatures &feature);
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::FontFeatureLigatures &feature);
enum FontFeaturePosition {
    PositionNormal,
    PositionSuper,
    PositionSub
};
FontFeaturePosition parseFontFeaturePosition(const QString &value, FontFeaturePosition feature);
QString writeFontFeaturePosition(const FontFeaturePosition &value);
QStringList fontFeaturesPosition(const FontFeaturePosition &feature, const int start, const int end);

Q_ENUM_NS(FontFeaturePosition)
enum FontFeatureCaps {
    CapsNormal,
    CapsSmall,
    CapsAllSmall,
    CapsPetite,
    CapsAllPetite,
    CapsUnicase,
    CapsTitling
};
FontFeatureCaps parseFontFeatureCaps(const QString &value, FontFeatureCaps feature);
QString writeFontFeatureCaps(const FontFeatureCaps &value);
QStringList fontFeaturesCaps(const FontFeatureCaps &feature, const int start, const int end);

Q_ENUM_NS(FontFeatureCaps)

enum NumericFigureStyle {
    NumericFigureStyleNormal,
    NumericFigureStyleLining,
    NumericFigureStyleOld
};
Q_ENUM_NS(NumericFigureStyle)

enum NumericFigureSpacing {
    NumericFigureSpacingNormal,
    NumericFigureSpacingProportional,
    NumericFigureSpacingTabular
};
Q_ENUM_NS(NumericFigureSpacing)
enum NumericFractions {
    NumericFractionsNormal,
    NumericFractionsDiagonal,
    NumericFractionsStacked
};
Q_ENUM_NS(NumericFractions)
struct FontFeatureNumeric : public boost::equality_comparable<FontFeatureNumeric> {

    NumericFigureStyle style = NumericFigureStyleNormal;
    NumericFigureSpacing spacing = NumericFigureSpacingNormal;
    NumericFractions fractions = NumericFractionsNormal;
    bool ordinals = false;
    bool slashedZero = false;
    bool operator==(const FontFeatureNumeric & other) const {
        return (other.style == style
                && other.spacing == spacing
                && other.fractions == fractions
                && other.ordinals == ordinals
                && other.slashedZero == slashedZero);
    }

    QStringList fontFeatures(const int start, const int end) {
        QStringList list;
        const QString length = QString("[%1:%2]").arg(start).arg(end);
        switch (style) {
        case NumericFigureStyleLining:
            list << "lnum" + length + "=1";
            break;
        case NumericFigureStyleOld:
            list << "onum" + length + "=1";
            break;
        default:
            break;
        }
        switch (spacing) {
        case NumericFigureSpacingProportional:
            list << "pnum" + length + "=1";
            break;
        case NumericFigureSpacingTabular:
            list << "tnum" + length + "=1";
            break;
        default:
            break;
        }
        switch (fractions) {
        case NumericFractionsDiagonal:
            list << "frac" + length + "=1";
            break;
        case NumericFractionsStacked:
            list << "afrc" + length + "=1";
            break;
        default:
            break;
        }
        if (ordinals) {
            list << "ordn" + length + "=1";
        }
        if (slashedZero) {
            list << "zero" + length + "=1";
        }
        return list;
    }
};

FontFeatureNumeric parseFontFeatureNumeric(const QString &value, FontFeatureNumeric features);
QString writeFontFeatureNumeric(const FontFeatureNumeric &feature);
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::FontFeatureNumeric &feature);
enum EastAsianVariant {
    EastAsianVariantNormal,
    EastAsianJis78,
    EastAsianJis83,
    EastAsianJis90,
    EastAsianJis04,
    EastAsianSimplified,
    EastAsianTraditional
};
Q_ENUM_NS(EastAsianVariant)
enum EastAsianWidth {
    EastAsiantNormalWidth,
    EastAsianFullWidth,
    EastAsianProportionalWidth
};
Q_ENUM_NS(EastAsianWidth)
struct FontFeatureEastAsian : public boost::equality_comparable<FontFeatureEastAsian> {
    EastAsianVariant variant = EastAsianVariantNormal;
    EastAsianWidth width = EastAsiantNormalWidth;
    bool ruby = false;
    bool operator==(const FontFeatureEastAsian & other) const {
        return (other.variant == variant
                && other.width == width
                && other.ruby == ruby);
    }

    QStringList fontFeatures(const int start, const int end) {
        QStringList list;
        const QString length = QString("[%1:%2]").arg(start).arg(end);
        switch (variant) {
        case EastAsianJis78:
            list << "jp78" + length + "=1";
            break;
        case EastAsianJis83:
            list << "jp83" + length + "=1";
            break;
        case EastAsianJis90:
            list << "jp90" + length + "=1";
            break;
        case EastAsianJis04:
            list << "jp04" + length + "=1";
            break;
        case EastAsianSimplified:
            list << "smpl" + length + "=1";
            break;
        case EastAsianTraditional:
            list << "trad" + length + "=1";
            break;
        default:
            break;
        }
        switch (width) {
        case EastAsianFullWidth:
            list << "fwid" + length + "=1";
            break;
        case EastAsianProportionalWidth:
            list << "pwid" + length + "=1";
            break;
        default:
            break;
        }
        if (ruby) {
            list << "ruby" + length + "=1";
        }
        return list;
    }
};
FontFeatureEastAsian parseFontFeatureEastAsian(const QString &value, FontFeatureEastAsian features);
QString writeFontFeatureEastAsian(const FontFeatureEastAsian &feature);
QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::FontFeatureEastAsian &feature);

struct TextUnderlinePosition : public boost::equality_comparable<TextUnderlinePosition> {
    TextDecorationUnderlinePosition horizontalPosition = UnderlineAuto;
    TextDecorationUnderlinePosition verticalPosition = UnderlineLeft;

    bool operator==(const TextUnderlinePosition & other) const {
        return (other.horizontalPosition == horizontalPosition
                && other.verticalPosition == verticalPosition);
    }

};

struct ResolutionHandler {

    ResolutionHandler(qreal _xRes = 72.0, qreal _yRes = 72.0, bool _roundToPixelHorizontal = false, bool _roundToPixelVertical = false)
        : xRes(_xRes), yRes(_yRes), roundToPixelHorizontal(_roundToPixelHorizontal), roundToPixelVertical(_roundToPixelVertical) {}

    qreal xRes = 72.0;
    qreal yRes = 72.0;
    bool roundToPixelHorizontal = false;
    bool roundToPixelVertical = false;

    const qreal freeTypePixel = 64.0;
    const qreal pointInInch = 72.0;

    qreal freeTypePixelToPointFactor(const bool x = true) const;

    QTransform freeTypeToPixelTransform() const;
    QTransform freeTypeToPointTransform() const;

    qreal pointToPixelFactor(const bool x = true) const;
    QTransform pointToPixel() const;

    qreal pixelToPointFactor(const bool x = true) const;
    QTransform pixelToPoint() const;

    QPointF adjust(const QPointF point) const;

    QPointF adjustFloor(const QPointF point) const;
    QPointF adjustCeil(const QPointF point) const;

    QPointF adjustWithOffset(const QPointF point, const QPointF offset) const;

    QRectF adjust(const QRectF rect) const;
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgText::TextUnderlinePosition &position);

}

Q_DECLARE_METATYPE(KoSvgText::CssLengthPercentage)
Q_DECLARE_METATYPE(KoSvgText::AutoValue)
Q_DECLARE_METATYPE(KoSvgText::AutoLengthPercentage)
Q_DECLARE_METATYPE(KoSvgText::TextDecorations)
Q_DECLARE_METATYPE(KoSvgText::HangingPunctuations)
Q_DECLARE_METATYPE(KoSvgText::TextSpaceTrims)
Q_DECLARE_METATYPE(KoSvgText::BackgroundProperty)
Q_DECLARE_METATYPE(KoSvgText::StrokeProperty)
Q_DECLARE_METATYPE(KoSvgText::TextTransformInfo)
Q_DECLARE_METATYPE(KoSvgText::TextIndentInfo)
Q_DECLARE_METATYPE(KoSvgText::TabSizeInfo)
Q_DECLARE_METATYPE(KoSvgText::LineHeightInfo)
Q_DECLARE_METATYPE(KoSvgText::CssFontStyleData)

Q_DECLARE_METATYPE(KoSvgText::FontFamilyAxis)
Q_DECLARE_METATYPE(KoSvgText::FontFamilyStyleInfo)

Q_DECLARE_METATYPE(KoSvgText::FontFeatureLigatures)
Q_DECLARE_METATYPE(KoSvgText::FontFeatureNumeric)
Q_DECLARE_METATYPE(KoSvgText::FontFeatureEastAsian)
Q_DECLARE_METATYPE(KoSvgText::TextUnderlinePosition)

Q_DECLARE_METATYPE(KoSvgText::FontMetrics)

#endif
