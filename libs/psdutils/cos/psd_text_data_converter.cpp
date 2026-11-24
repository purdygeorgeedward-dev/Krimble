/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "psd_text_data_converter.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#include <KoCSSFontInfo.h>
#include <KoSvgTextProperties.h>
#include <KoPathSegment.h>
#include <KoPathPoint.h>
#include <KoFontRegistry.h>
#include <KoShape.h>
#include <KoPathShape.h>

#include <KoColorSpace.h>
#include <KoColor.h>

#include <QBuffer>
#include <QStringList>
#include <QTransform>
#include <QXmlStreamWriter>
#include <QtMath>
struct PsdTextDataConverter::Private {

    QStringList errors;
    QStringList warnings;

    void clearErrors() {
        errors.clear();
        warnings.clear();
    }
};
PsdTextDataConverter::PsdTextDataConverter()
    : d(new Private)
{

}

PsdTextDataConverter::~PsdTextDataConverter()
{

}

QColor PsdTextDataConverter::colorFromPSDStyleSheet(QVariantHash color, const KoColorSpace *imageCs) {
    QColor c(Qt::black);
    if (color.keys().contains("/Color")) {
        color = color["/Color"].toHash();
    }
    QDomDocument doc;
    QDomElement root;
    QVariantList values = color.value("/Values").toList();
    if (color.value("/Type").toInt() == 0) {
        root = doc.createElement("Gray");
        root.setAttribute("g", values.at(1).toDouble());
    } else if (color.value("/Type").toInt() == 2) {
        root = doc.createElement("CMYK");
        root.setAttribute("c", values.value(1).toDouble());
        root.setAttribute("m", values.value(2).toDouble());
        root.setAttribute("y", values.value(3).toDouble());
        root.setAttribute("k", values.value(4).toDouble());
    } else if (color.value("/Type").toInt() == 3) {
        root = doc.createElement("Lab");
        root.setAttribute("L", values.value(1).toDouble());
        root.setAttribute("a", values.value(2).toDouble());
        root.setAttribute("b", values.value(3).toDouble());
    } else if (color.value("/Type").toInt() == 1) {
        root = doc.createElement("RGB");
        root.setAttribute("r", values.value(1).toDouble());
        root.setAttribute("g", values.value(2).toDouble());
        root.setAttribute("b", values.value(3).toDouble());
    }
    KoColor final = KoColor::fromXML(root, "U8");
    if (final.colorSpace()->colorModelId() == imageCs->colorModelId()) {
        final.setProfile(imageCs->profile());
    }
    final.toQColor(&c);
    return c;
}

static QHash <int, QString> psdLanguageMap {
    {0, "en-US"},
    {1, "fi"},
    {2, "fr"},
    {3, "fr-CA"},
    {4, "de"},
    {5, "de-1901"},
    {6, "gsw"},
    {7, "it"},
    {8, "nb"},
    {9, "nn"},
    {10, "pt"},
    {11, "pt-BR"},
    {12, "es"},
    {13, "sv"},
    {14, "en-UK"},
    {15, "nl"},
    {16, "da"},
    {18, "ru"},
    {22, "cs"},
    {23, "pl"},
    {25, "el"},
    {26, "tr"},
    {28, "hu"},
};

QString PsdTextDataConverter::stylesForPSDStyleSheet(QString &lang, QVariantHash PSDStyleSheet, QMap<int, KoCSSFontInfo> fontNames, QTransform scale, const KoColorSpace *imageCs) {
    QStringList styles;

    QStringList unsupportedStyles;

    int weight = 400;
    bool italic = false;
    QStringList textDecor;
    QStringList baselineShift;
    QStringList fontVariantLigatures;
    QStringList fontVariantNumeric;
    QStringList fontVariantCaps;
    QStringList fontVariantEastAsian;
    QStringList fontFeatureSettings;
    QString underlinePos;
    for (int i=0; i < PSDStyleSheet.keys().size(); i++) {
        const QString key = PSDStyleSheet.keys().at(i);
        if (key == "/Font") {
            KoCSSFontInfo fontInfo = fontNames.value(PSDStyleSheet.value(key).toInt());
            weight = fontInfo.weight;
            italic = italic? true: fontInfo.slantMode != QFont::StyleNormal;
            styles.append("font-family:"+fontInfo.families.join(","));
            if (fontInfo.width != 100) {
                styles.append("font-width:"+QString::number(fontInfo.width));
            }
            continue;
        } else if (key == "/FontSize") {
            double val = PSDStyleSheet.value(key).toDouble();
            val = scale.map(QPointF(val, val)).y();
            styles.append("font-size:"+QString::number(val));
            continue;
        } else if (key == "/AutoKerning" || key == "/AutoKern") {
            if (!PSDStyleSheet.value(key).toBool()) {
                styles.append("font-kerning: none");
            }
            continue;
        } else if (key == "/Kerning") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/FauxBold") {
            if (PSDStyleSheet.value(key).toBool()) {
                weight = 700;
            }
            continue;
        } else if (key == "/FauxItalic") {
            if (PSDStyleSheet.value(key).toBool()) {
                italic = true;
            }
            continue;
        } else if (key == "/Leading") {
            bool autoleading = true;
            if (PSDStyleSheet.keys().contains("AutoLeading")) {
                autoleading = PSDStyleSheet.value("AutoLeading").toBool();
            }
            if (!autoleading) {
                double fontSize = PSDStyleSheet.value("FontSize").toDouble();
                double val = PSDStyleSheet.value(key).toDouble();
                styles.append("line-height:"+QString::number(val/fontSize));
            }
            continue;
        } else if (key == "/HorizontalScale" || key == "/VerticalScale") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/Tracking") {
            double letterSpacing = (0.001 * PSDStyleSheet.value(key).toDouble());
            styles.append("letter-spacing:"+QString::number(letterSpacing)+"em");
            continue;
        } else if (key == "/BaselineShift") {
            if (PSDStyleSheet.value(key).toDouble() > 0) {
                double val = PSDStyleSheet.value(key).toDouble();
                val = scale.map(QPointF(val, val)).y();
                baselineShift.append(QString::number(val));
            }
            continue;
        } else if (key == "/FontCaps") {
            switch (PSDStyleSheet.value(key).toInt()) {
            case 0:
                break;
            case 1:
                fontVariantCaps.append("all-small-caps");
                break;
            case 2:
                styles.append("text-transform:uppercase");
                break;
            default:
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        } else if (key == "/FontBaseline") {
            switch (PSDStyleSheet.value(key).toInt()) {
            case 0:
                break;
            case 1:
                baselineShift.append("super");
                break;
            case 2:
                baselineShift.append("sub");
                break;
            default:
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        } else if (key == "/FontOTPosition") {
            switch (PSDStyleSheet.value(key).toInt()) {
            case 0:
                break;
            case 1:
                styles.append("font-variant-position:super");
                break;
            case 2:
                styles.append("font-variant-position:sub");
                break;
            case 3:
                fontFeatureSettings.append("'numr' 1");
                break;
            case 4:
                fontFeatureSettings.append("'dnum' 1");
                break;
            default:
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        } else if (key == "/Underline") {
            if (PSDStyleSheet.value(key).toBool()) {
                textDecor.append("underline");
            }
            continue;
        }  else if (key == "/UnderlinePosition") {
            switch (PSDStyleSheet.value(key).toInt()) {
            case 0:
                break;
            case 1:
                textDecor.append("underline");
                underlinePos = "auto left";
                break;
            case 2:
                textDecor.append("underline");
                underlinePos = "auto right";
                break;
            default:
                d->warnings << QString("Unknown value for %1: %1").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        } else if (key == "/YUnderline") {
            if (PSDStyleSheet.value(key).toInt() == 1) {
                underlinePos = "auto left";
            } else if (PSDStyleSheet.value(key).toInt() == 0) {
                underlinePos = "auto right";
            }
            continue;
        } else if (key == "/Strikethrough" || key == "/StrikethroughPosition") {
            if (PSDStyleSheet.value(key).toBool()) {
                textDecor.append("line-through");
            }
            continue;
        } else if (key == "/Ligatures") {
            if (!PSDStyleSheet.value(key).toBool()) {
                fontVariantLigatures.append("no-common-ligatures");
            }
            continue;
        } else if (key == "/DLigatures" || key == "/DiscretionaryLigatures" || key == "/AlternateLigatures") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontVariantLigatures.append("discretionary-ligatures");
            }
            continue;
        } else if (key == "/ContextualLigatures") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontVariantLigatures.append("contextual");
            }
            continue;
        } else if (key == "/Fractions") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontVariantNumeric.append("diagonal-fractions");
            }
            continue;
        } else if (key == "/Ordinals") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontVariantNumeric.append("ordinal");
            }
            continue;
        } else if (key == "/Swash") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontFeatureSettings.append("'swsh' 1");
            }
            continue;
        } else if (key == "/Titling") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontVariantCaps.append("titling-caps");
            }
            continue;
        } else if (key == "/StylisticAlternates") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontFeatureSettings.append("'salt' 1");
            }
            continue;
        } else if (key == "/Ornaments") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontFeatureSettings.append("'ornm' 1");
            }
            continue;
        }  else if (key == "/OldStyle") {
            if (PSDStyleSheet.value(key).toBool() && !fontVariantNumeric.contains("oldstyle-nums")) {
                fontVariantNumeric.append("oldstyle-nums");
            }
            continue;
        } else if (key == "/FigureStyle") {
            switch (PSDStyleSheet.value(key).toInt()) {
            case 0:
                break;
            case 1:
                fontVariantNumeric.append("tabular-nums");
                fontVariantNumeric.append("lining-nums");
                break;
            case 2:
                fontVariantNumeric.append("proportional-nums");
                fontVariantNumeric.append("oldstyle-nums");
                break;
            case 3:
                fontVariantNumeric.append("proportional-nums");
                fontVariantNumeric.append("lining-nums");
                break;
            case 4:
                fontVariantNumeric.append("tabular-nums");
                fontVariantNumeric.append("oldstyle-nums");
                break;
            default:
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        } else if (key == "/Italics") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontFeatureSettings.append("'ital' 1");
            }
            continue;
        } else if (key == "/BaselineDirection") {
            int val = PSDStyleSheet.value(key).toInt();
            if (val == 1) {
                styles.append("text-orientation: upright");
            } else if (val == 2) {
                styles.append("text-orientation: mixed");
            } else if (val == 3) {
                styles.append("text-combine-upright: all");
            } else {
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        } else if (key == "/Tsume" || key == "/LeftAki" || key == "/RightAki" || key == "/JiDori") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/StyleRunAlignment") {
            QString dominantBaseline;
            switch(PSDStyleSheet.value(key).toInt()) {
            case 3:
                dominantBaseline = "alphabetic";
                break;
            case 2:
                dominantBaseline = "center";
                break;
            case 0:
                dominantBaseline = "ideographic";
                break;
            case 4:
                dominantBaseline = "text-top";
                break;
            case 1:
                dominantBaseline = "text-bottom";
                break;
            default:
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
                dominantBaseline = QString();
            }
            if (!dominantBaseline.isEmpty()) {
                styles.append("dominant-baseline: "+dominantBaseline);
                styles.append("alignment-baseline: "+dominantBaseline);
            }
            continue;
        } else if (key == "/Language") {
            int val = PSDStyleSheet.value(key).toInt();
            if (psdLanguageMap.keys().contains(val)) {
                lang = psdLanguageMap.value(val);
            } else {
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        }  else if (key == "/ProportionalMetrics") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontFeatureSettings.append("'palt' 1");
            }
            continue;
        } else if (key == "/Kana") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontFeatureSettings.append("'hkna' 1");
            }
            continue;
        } else if (key == "/Ruby") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontVariantEastAsian.append("ruby");
            }
        } else if (key == "/JapaneseAlternateFeature") {
            int val = PSDStyleSheet.value(key).toInt();
            if (val == 0) {
                continue;
            } else if (val == 1) {
                fontVariantEastAsian.append("traditional");
            } else if (val == 2) {
                fontFeatureSettings.append("'expt' 1");
            } else if (val == 3) {
                fontVariantEastAsian.append("jis78");
            } else {
                d->warnings << QString("Unknown value for %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
            continue;
        } else if (key == "/NoBreak") {
            if (PSDStyleSheet.value(key).toBool()) {
                styles.append("word-break: keep-all");
            }
            continue;
        } else if (key == "/DirOverride") {
            QString dir = PSDStyleSheet.value(key).toBool()? "rtl": "ltr";
            if (PSDStyleSheet.value(key).toBool()) {
                styles.append("direction: "+dir);
                styles.append("unicode-bidi: isolate");
            }
            continue;
        }  else if (key == "/FillColor") {
            bool fill = true;
            if (PSDStyleSheet.keys().contains("/FillFlag")) {
                fill = PSDStyleSheet.value("/FillFlag").toBool();
            }
            if (fill) {
                QVariantHash color = PSDStyleSheet.value(key).toHash();
                styles.append("fill:"+colorFromPSDStyleSheet(color, imageCs).name());
            } else {
                styles.append("fill:none");
            }
        } else if (key == "/StrokeColor") {
            bool fill = true;
            if (PSDStyleSheet.keys().contains("/StrokeFlag")) {
                fill = PSDStyleSheet.value("/StrokeFlag").toBool();
            }
            if (fill) {
                QVariantHash color = PSDStyleSheet.value(key).toHash();
                styles.append("stroke:"+colorFromPSDStyleSheet(color, imageCs).name());
            } else {
                styles.append("stroke:none");
            }
            continue;
        } else if (key == "/OutlineWidth" || key == "/LineWidth") {
            double val = PSDStyleSheet.value(key).toDouble();
            val = scale.map(QPointF(val, val)).y();
            styles.append("stroke-width:"+QString::number(val));
        } else if (key == "/FillFirst") {
            if (PSDStyleSheet.value(key).toBool()) {
                styles.append("paint-order: fill");
            }
            continue;
        } else if (key == "/HindiNumbers") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/Kashida") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/DiacriticPos") {
            unsupportedStyles << key;
            continue;
        }  else if (key == "/SlashedZero") {
            if (PSDStyleSheet.value(key).toBool()) {
                fontVariantNumeric.append("slashed-zero");
            }
            continue;
        } else if (key == "/StylisticSets") {
            int flags = PSDStyleSheet.value(key).toInt();
            for (int i = 1; i <= 20; i++) {
                const int bit = 2^(i-1);
                const QString tag = i > 9? QString("ss%1").arg(i):QString("ss0%1").arg(i);
                if (flags & bit) {
                    fontFeatureSettings.append(QString("\'%1\' 1").arg(tag));
                }
            }
            continue;
        } else if (key == "/LineCap") {
            switch (PSDStyleSheet.value(key).toInt()) {
            case 0:
                styles.append("stroke-linecap: butt");
                break;
            case 1:
                styles.append("stroke-linecap: round");
                break;
            case 2:
                styles.append("stroke-linecap: square");
                break;
            default:
                styles.append("stroke-linecap: butt");
            }
        } else if (key == "/LineJoin") {
            switch (PSDStyleSheet.value(key).toInt()) {
            case 0:
                styles.append("stroke-linejoin: miter");
                break;
            case 1:
                styles.append("stroke-linejoin: round");
                break;
            case 2:
                styles.append("stroke-linejoin: bevel");
                break;
            default:
                styles.append("stroke-linejoin: miter");
            }
        } else if (key == "/MiterLimit") {
            styles.append("stroke-miterlimit: "+PSDStyleSheet.value(key).toString());
        } else if (key == "/LineDashOffset") {
            styles.append("stroke-dashoffset: "+PSDStyleSheet.value(key).toString());
        } else if (key == "/EnableWariChu" || key == "/WariChuWidowAmount" || key == "/WariChuLineGap" || key == "/WariChuJustification"
                   || key == "/WariChuOrphanAmount" || key == "/WariChuLineCount" || key == "/WariChuSubLineAmount") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/TCYUpDownAdjustment" || key == "/TCYLeftRightAdjustment") {
            unsupportedStyles << key;
            continue;
        }  else if (key == "/Type1EncodingNames" || key == "/ConnectionForms") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/FillOverPrint" || key == "/StrokeOverPrint" || key == "/Blend") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/UnderlineOffset") {
            unsupportedStyles << key;
            continue;
        } else {
            if (key != "/FillFlag" && key != "/StrokeFlag" && key != "/AutoLeading") {
                d->warnings << QString("Unknown PSD character stylesheet style key, %1: %2").arg(key).arg(PSDStyleSheet.value(key).toString());
            }
        }
    }
    if (weight != 400) {
        styles.append("font-weight:"+QString::number(weight));
    }
    if (italic) {
        styles.append("font-style:italic");
    }
    if (!textDecor.isEmpty()) {
        styles.append("text-decoration:"+textDecor.join(" "));
    }
    if (!baselineShift.isEmpty()) {
        styles.append("baseline-shift:"+baselineShift.join(" "));
    }
    if (!fontVariantLigatures.isEmpty()) {
        styles.append("font-variant-ligatures:"+fontVariantLigatures.join(" "));
    }
    if (!fontVariantNumeric.isEmpty()) {
        styles.append("font-variant-numeric:"+fontVariantNumeric.join(" "));
    }
    if (!fontVariantCaps.isEmpty()) {
        styles.append("font-variant-caps:"+fontVariantCaps.join(" "));
    }
    if (!fontVariantEastAsian.isEmpty()) {
        styles.append("font-variant-east-asian:"+fontVariantEastAsian.join(" "));
    }
    if (!fontFeatureSettings.isEmpty()) {
        styles.append("font-feature-settings:"+fontFeatureSettings.join(", "));
    }
    if (!underlinePos.isEmpty()) {
        styles.append("text-decoration-position:"+underlinePos);
    }
    d->warnings << QString("Unsupported styles: %1").arg(unsupportedStyles.join(","));
    return styles.join("; ");
}

QString PsdTextDataConverter::stylesForPSDParagraphSheet(QVariantHash PSDParagraphSheet, QString &lang, QMap<int, KoCSSFontInfo> fontNames, QTransform scaleToPt, const KoColorSpace *imageCs) {
    QStringList styles;
    QStringList unsupportedStyles;

    for (int i=0; i < PSDParagraphSheet.keys().size(); i++) {
        const QString key = PSDParagraphSheet.keys().at(i);
        double val = PSDParagraphSheet.value(key).toDouble();
        if (key == "/Justification") {
            QString textAlign = "start";
            QString textAnchor = "start";
            switch (PSDParagraphSheet.value(key).toInt()) {
            case 0:
                textAlign = "start";
                textAnchor = "start";
                break;
            case 1:
                textAlign = "end";
                textAnchor = "end";
                break;
            case 2:
                textAlign = "center";
                textAnchor = "middle";
                break;
            case 3:
                textAlign = "justify start";
                textAnchor = "start";
                break;
            case 4:
                textAlign = "justify end";
                textAnchor = "end";
                break;
            case 5:
                textAlign = "justify center";
                textAnchor = "middle";
                break;
            case 6:
                textAlign = "justify";
                textAnchor = "middle";
                break;
            default:
                textAlign = "start";
            }

            styles.append("text-align:"+textAlign);
            styles.append("text-anchor:"+textAnchor);
        } else if (key == "/FirstLineIndent") {
            val = scaleToPt.map(QPointF(val, val)).x();
            styles.append("text-indent:"+QString::number(val));
            continue;
        } else if (key == "/StartIndent") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/EndIndent") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/SpaceBefore") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/SpaceAfter") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/AutoHyphenate") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/HyphenatedWordSize") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/PreHyphen") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/PostHyphen") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/ConsecutiveHyphens") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/HyphenateCapitalized") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/HyphenationPreference") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/SingleWordJustification") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/Zone") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/WordSpacing") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/LetterSpacing") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/GlyphSpacing") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/AutoLeading") {
            styles.append("line-height:"+QString::number(val));
            continue;
        } else if (key == "/LeadingType") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/Hanging") {
            continue;
        } else if (key == "/Burasagari" || key == "/BurasagariType") {
            if (PSDParagraphSheet.value(key).toBool()) {
                styles.append("hanging-punctuation:allow-end");
            }
            continue;
        } else if (key == "/Kinsoku") {
            unsupportedStyles << key;
            continue;
        }  else if (key == "/KinsokuOrder") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/EveryLineComposer") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/ComposerEngine") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/KurikaeshiMojiShori") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/MojiKumiTable") {
            unsupportedStyles << key;
            continue;
        }  else if (key == "/DropCaps") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/TabStops" || key == "/AutoTCY" || key == "/KeepTogether" ) {
            unsupportedStyles << key;
            continue;
        } else if (key == "/ParagraphDirection") {
            switch (PSDParagraphSheet.value(key).toInt()) {
            case 1:
                styles.append("direction:rtl");
                break;
            case 0:
                styles.append("direction:ltr");
                break;
            default:
                break;
            }
        } else if (key == "/DefaultTabWidth") {
            unsupportedStyles << key;
            continue;
        } else if (key == "/DefaultStyle") {
            styles.append(stylesForPSDStyleSheet(lang, PSDParagraphSheet.value(key).toHash(), fontNames, scaleToPt, imageCs));
        } else {
            d->warnings << QString("Unknown PSD character stylesheet style key, %1: %2").arg(key).arg(PSDParagraphSheet.value(key).toString());
        }
    }
    d->warnings << QString("Unsupported paragraph styles: %1").arg(unsupportedStyles.join(","));

    return styles.join("; ");
}

bool PsdTextDataConverter::convertPSDTextEngineDataToSVG(const QVariantHash tySh,
                                                                  const QVariantHash txt2,
                                                                  const KoColorSpace *imageCs,
                                                                  const int textIndex,
                                                                  QString *svgText,
                                                                  QString *svgStyles,
                                                                  QPointF &offset,
                                                                  bool &offsetByAscent,
                                                                  bool &isHorizontal,
                                                                  QTransform scaleToPt)
{

    QVariantHash root = tySh;
    bool loadFallback = txt2.isEmpty();
    const QVariantHash docObjects = txt2.value("/DocumentObjects").toHash();

    QVariantHash textObject = docObjects.value("/TextObjects").toList().value(textIndex).toHash();
    if (textObject.isEmpty() || loadFallback) {
        textObject = root["/EngineDict"].toHash();
        loadFallback = true;
    }
    if (textObject.isEmpty()) {
        d->errors << "No engine dict found in PSD engine data";
        return false;
    }

    QMap<int, KoCSSFontInfo> fontNames;
    QVariantHash resourceDict = loadFallback? root.value("/DocumentResources").toHash(): txt2.value("/DocumentResources").toHash();
    if (resourceDict.isEmpty()) {
        d->errors << "No engine dict found in PSD engine data";
        return false;
    } else {
        QVariantList fonts = loadFallback? resourceDict.value("/FontSet").toList()
                                         : resourceDict.value("/FontSet").toHash().value("/Resources").toList();
        for (int i = 0; i < fonts.size(); i++) {
            QVariantHash font = loadFallback? fonts.value(i).toHash()
                                            : fonts.value(i).toHash().value("/Resource").toHash().value("/Identifier").toHash();
            QString postScriptName = font.value("/Name").toString();
            QString foundPostScriptName;
            KoCSSFontInfo fontInfo = KoFontRegistry::instance()->getCssDataForPostScriptName(postScriptName,
                                                                    &foundPostScriptName);

            if (postScriptName != foundPostScriptName) {
                fontInfo.families = QStringList({"sans-serif"});
                d->errors << QString("Font %1 not found, substituting %2").arg(postScriptName).arg(fontInfo.families.join(","));
            }
            fontNames.insert(i, fontInfo);
        }
    }

    QString inlineSizeString;
    QRectF bounds;

    QScopedPointer<KoPathShape> textShape;
    double textPathStartOffset = -3;
    double shapePadding = 0.0;
    int textType = 0;
    bool reversed = false;
    if (loadFallback) {
        QVariantHash rendered = textObject.value("/Rendered").toHash();
        if (!rendered.isEmpty()) {
            QVariantHash shapeChild = rendered.value("/Shapes").toHash().value("/Children").toList()[0].toHash();
            textType = shapeChild.value("/ShapeType").toInt();
            if (textType == 1) {
                QVariantList BoxBounds = shapeChild.value("/Cookie").toHash().value("/Photoshop").toHash().value("/BoxBounds").toList();
                if (BoxBounds.size() == 4) {
                    bounds = QRectF(BoxBounds[0].toDouble(), BoxBounds[1].toDouble(), BoxBounds[2].toDouble(), BoxBounds[3].toDouble());
                    bounds = scaleToPt.mapRect(bounds);
                    if (isHorizontal) {
                        inlineSizeString = " inline-size:"+QString::number(bounds.width())+";";
                    } else {
                        inlineSizeString = " inline-size:"+QString::number(bounds.height())+";";
                    }
                }
            }
        }
    } else {
        QVariantHash view = textObject.value("/View").toHash();
        QVariantList frames = view.value("/Frames").toList();
        if (!frames.isEmpty()) {
            int textFrameIndex = view.value("/Frames").toList().value(0).toHash().value("/Resource").toInt();
            QVariantList textFrameSet = resourceDict.value("/TextFrameSet").toHash().value("/Resources").toList();
            QVariantHash textFrame = textFrameSet.at(textFrameIndex).toHash().value("/Resource").toHash();

            if (!textFrame.isEmpty()) {
                textType = textFrame["/Data"].toHash()["/Type"].toInt();

                if (textType > 0) {
                    KoPathShape *textCurve = new KoPathShape();
                    QVariantHash data = textFrame.value("/Data").toHash();
                    QVariantList points = textFrame.value("/Bezier").toHash().value("/Points").toList();
                    QVariantList range = data.value("/TextOnPathTRange").toList();
                    QVariantList fm = data.value("/FrameMatrix").toList();
                    shapePadding = data.value("/Spacing").toDouble();
                    QVariantHash pathData = data.value("/PathData").toHash();
                    reversed = pathData.value("/Flip").toBool();

                    QVariant lineOrientation = data.value("/LineOrientation");
                    if (!lineOrientation.isNull()) {
                        if (lineOrientation.toInt() == 2) {
                            isHorizontal = false;
                        }
                    }
                    QTransform frameMatrix = scaleToPt;
                    if (fm.size() == 6) {
                        frameMatrix = QTransform(fm[0].toDouble(), fm[1].toDouble(), fm[2].toDouble(), fm[3].toDouble(), fm[4].toDouble(), fm[5].toDouble());
                        frameMatrix = frameMatrix * scaleToPt;
                    }

                    int length = points.size()/8;

                    QPointF startPoint;
                    QPointF endPoint;
                    for (int i = 0; i < length; i++) {
                        int iAdjust = i*8;
                        QPointF p1(points[iAdjust  ].toDouble(), points[iAdjust+1].toDouble());
                        QPointF p2(points[iAdjust+2].toDouble(), points[iAdjust+3].toDouble());
                        QPointF p3(points[iAdjust+4].toDouble(), points[iAdjust+5].toDouble());
                        QPointF p4(points[iAdjust+6].toDouble(), points[iAdjust+7].toDouble());

                        if (i == 0 || endPoint != frameMatrix.map(p1)) {
                            if (endPoint == startPoint && i > 0) {
                                textCurve->closeMerge();
                            }
                            textCurve->moveTo(frameMatrix.map(p1));
                            startPoint = frameMatrix.map(p1);
                        }
                        if (p1==p2 && p3==p4) {
                            textCurve->lineTo(frameMatrix.map(p4));
                        } else {
                            textCurve->curveTo(frameMatrix.map(p2), frameMatrix.map(p3), frameMatrix.map(p4));
                        }
                        endPoint = frameMatrix.map(p4);
                    }
                    if (points.size() > 8) {
                        if (endPoint == startPoint) {
                            textCurve->closeMerge();
                        }
                        textShape.reset(textCurve);
                    } else {
                        delete textCurve;
                    }
                    if (!range.isEmpty()) {
                        textPathStartOffset = range[0].toDouble();
                        int segment = qFloor(textPathStartOffset);
                        double t = textPathStartOffset - segment;
                        double length = 0;
                        double totalLength = 0;
                        for (int i=0; i<textShape->subpathPointCount(0); i++) {
                            double l = textShape->segmentByIndex(KoPathPointIndex(0, i)).length();
                            totalLength += l;
                            if (i < segment) {
                                length += l;
                            } else if (i == segment) {
                                length += textShape->segmentByIndex(KoPathPointIndex(0, i)).lengthAt(t);
                            }
                        }
                        textPathStartOffset = (length/totalLength) * 100.0;
                    }
                }
            }
        }
    }
    QString paragraphStyle = isHorizontal? "writing-mode: horizontal-tb;": "writing-mode: vertical-rl;";
    paragraphStyle += " white-space: pre-wrap;";

    QBuffer svgBuffer;
    QBuffer styleBuffer;
    svgBuffer.open(QIODevice::WriteOnly);
    styleBuffer.open(QIODevice::WriteOnly);

    QXmlStreamWriter svgWriter(&svgBuffer);
    QXmlStreamWriter stylesWriter(&styleBuffer);
    stylesWriter.writeStartElement("defs");
    if (bounds.isValid()) {
        stylesWriter.writeStartElement("rect");
        stylesWriter.writeAttribute("id", "bounds");
        stylesWriter.writeAttribute("x", QString::number(bounds.x()));
        stylesWriter.writeAttribute("y", QString::number(bounds.y()));
        stylesWriter.writeAttribute("width", QString::number(bounds.width()));
        stylesWriter.writeAttribute("height", QString::number(bounds.height()));
        stylesWriter.writeEndElement();
    }
    if (textShape) {
        stylesWriter.writeStartElement("path");
        stylesWriter.writeAttribute("id", "textShape");
        stylesWriter.writeAttribute("d", textShape->toString());
        stylesWriter.writeAttribute("opacity", "0");
        stylesWriter.writeAttribute("sodipodi:nodetypes", textShape->nodeTypes());
        stylesWriter.writeEndElement();
    }

    svgWriter.setAutoFormatting(false);

    svgWriter.writeStartElement("text");

    QVariantHash editor = loadFallback? textObject.value("/Editor").toHash() : textObject.value("/Model").toHash();
    QString text = "";
    if (editor.isEmpty()) {
        d->errors << "No editor dict found in PSD engine data";
        return false;
    } else {
        text = editor.value("/Text").toString();
        text.replace("\r", "\n");
        text.replace(QChar(0x03), "\n");
    }

    int antiAliasing = 0;
        antiAliasing = loadFallback? textObject.value("/AntiAlias").toInt()
                                   : textObject.value("/StorySheet").toHash().value("/AntiAlias").toInt();
    if (antiAliasing == 3) {
        svgWriter.writeAttribute("text-rendering", "auto");
    } else if (antiAliasing == 0) {
        svgWriter.writeAttribute("text-rendering", "OptimizeSpeed");
    }

    QVariantHash paragraphRun = loadFallback? textObject.value("/ParagraphRun").toHash() : editor.value("/ParagraphRun").toHash();
    if (!paragraphRun.isEmpty()) {
        QVariantList runArray = paragraphRun.value("/RunArray").toList();
        QString features = loadFallback? "/Properties": "/Features";
        QVariantHash style = loadFallback? runArray.value(0).toHash() : runArray.value(0).toHash().value("/RunData").toHash();
        QVariantHash parasheet = loadFallback? runArray.value(0).toHash()["/ParagraphSheet"].toHash():
                runArray.at(0).toHash()["/RunData"].toHash()["/ParagraphSheet"].toHash();
        QVariantHash styleSheet = parasheet[features].toHash();

        QString lang;
        QString styleString = stylesForPSDParagraphSheet(styleSheet, lang, fontNames, scaleToPt, imageCs);
        if (!lang.isEmpty()) {
            svgWriter.writeAttribute("xml:lang", lang);
        }
        if (textType < 2) {
            if (textShape) {
                offsetByAscent = false;
                paragraphStyle += " shape-inside:url(#textShape);";
                if (shapePadding > 0) {
                    QPointF sPadding = scaleToPt.map(QPointF(shapePadding, shapePadding));
                    paragraphStyle += " shape-padding:"+QString::number(sPadding.x())+";";
                }
            } else if (styleString.contains("text-align:justify") && bounds.isValid()) {
                offsetByAscent = false;
                paragraphStyle += " shape-inside:url(#bounds);";
            } else if (bounds.isValid()){
                offsetByAscent = true;
                offset = isHorizontal? bounds.topLeft(): bounds.topRight();
                if (styleString.contains("text-anchor:middle")) {
                    offset = isHorizontal? QPointF(bounds.center().x(), offset.y()):
                                           QPointF(offset.x(), bounds.center().y());
                } else if (styleString.contains("text-anchor:end")) {
                    offset = isHorizontal? QPointF(bounds.right(), offset.y()):
                                           QPointF(offset.x(), bounds.bottom());
                }
                paragraphStyle += inlineSizeString;
                svgWriter.writeAttribute("transform", QString("translate(%1, %2)").arg(offset.x()).arg(offset.y()));
            }
        }
        paragraphStyle += styleString;
        svgWriter.writeAttribute("style", paragraphStyle);

    }

    bool textPathCreated = false;
    if (textShape && textType == 2) {
        svgWriter.writeStartElement("textPath");
        textPathCreated = true;
        svgWriter.writeAttribute("path", textShape->toString());
        if (reversed) {
            svgWriter.writeAttribute("side", "right");
        }
        svgWriter.writeAttribute("startOffset", QString::number(textPathStartOffset)+"%");
    }

    QVariantHash styleRun = loadFallback? textObject.value("/StyleRun").toHash(): editor.value("/StyleRun").toHash();
    if (styleRun.isEmpty()) {
        d->errors << "No styleRun dict found in PSD engine data";
        return false;
    } else {
        QString features = loadFallback? "/StyleSheetData": "/Features";
        QVariantList runLengthArray = styleRun.value("/RunLengthArray").toList();
        QVariantList runArray = styleRun.value("/RunArray").toList();
        if (runArray.isEmpty()) {
            d->errors << "No styleRun dict found in PSD engine data";
            return false;
        } else {
            QVariantHash style = loadFallback? runArray.at(0).toHash() : runArray.at(0).toHash()["/RunData"].toHash();
            QVariantHash styleSheet = style.value("/StyleSheet").toHash().value(features).toHash();
            int length = 0;
            int pos = 0;
            for (int i = 0; i < runArray.size(); i++) {
                style = loadFallback? runArray.at(i).toHash() : runArray.at(i).toHash()["/RunData"].toHash();
                int l = loadFallback? runLengthArray.at(i).toInt(): runArray.at(i).toHash().value("/Length").toInt();

                QVariantHash newStyleSheet = style.value("/StyleSheet").toHash().value(features).toHash();
                if (newStyleSheet == styleSheet) {
                    length += l;
                } else {
                    svgWriter.writeStartElement("tspan");
                    QString lang;
                    svgWriter.writeAttribute("style", stylesForPSDStyleSheet(lang, styleSheet, fontNames, scaleToPt, imageCs));
                    if (!lang.isEmpty()) {
                        svgWriter.writeAttribute("xml:lang", lang);
                    }
                    svgWriter.writeCharacters(text.mid(pos, length));
                    svgWriter.writeEndElement();
                    styleSheet = newStyleSheet;
                    pos += length;
                    length = l;
                }
            }
            svgWriter.writeStartElement("tspan");
            QString lang;
            svgWriter.writeAttribute("style", stylesForPSDStyleSheet(lang, styleSheet, fontNames, scaleToPt, imageCs));
            if (!lang.isEmpty()) {
                svgWriter.writeAttribute("xml:lang", lang);
            }
            svgWriter.writeCharacters(text.mid(pos));
            svgWriter.writeEndElement();
        }
    }

    if (textPathCreated) {
        svgWriter.writeEndElement();
    }

    svgWriter.writeEndElement();
    stylesWriter.writeEndElement();

    if (svgWriter.hasError() || stylesWriter.hasError()) {
        d->errors << i18n("Unknown error writing SVG text element");
        return false;
    }
    *svgText = QString::fromUtf8(svgBuffer.data()).trimmed();
    *svgStyles = QString::fromUtf8(styleBuffer.data()).trimmed();

    return true;
}

void PsdTextDataConverter::gatherFonts(const QMap<QString, QString> cssStyles, const QString text, QVariantList &fontSet,
                 QVector<int> &lengths, QVector<int> &fontIndices) {
    if (cssStyles.contains("font-family")) {
        QStringList families = cssStyles.value("font-family").split(",");
        int fontSize = cssStyles.value("font-size", "10").toInt();
        int fontWeight = cssStyles.value("font-weight", "400").toInt();
        int fontWidth = cssStyles.value("font-stretch", "100").toInt();

        KoCSSFontInfo fontInfo;
        fontInfo.families = families;
        fontInfo.size = fontSize;
        fontInfo.weight = fontWeight;
        fontInfo.width = fontWidth;
        const std::vector<FT_FaceSP> faces = KoFontRegistry::instance()->facesForCSSValues(lengths, fontInfo,
                                                      text, 72, 72);

        for (uint i = 0; i < faces.size(); i++) {
            const FT_FaceSP &face = faces.at(static_cast<size_t>(i));
            QString postScriptName = face->family_name;
            if (FT_Get_Postscript_Name(face.data())) {
                postScriptName = FT_Get_Postscript_Name(face.data());
            }

            int fontIndex = -1;
            for(int j=0; j<fontSet.size(); j++) {
                if (fontSet[j].toHash()["/Name"] == postScriptName) {
                    fontIndex = j;
                    break;
                }
            }
            if (fontIndex < 0) {
                QVariantHash font;
                font["/Name"] = postScriptName;
                font["/Type"] = 1;
                fontSet.append(font);
                fontIndex = fontSet.size()-1;
            }
            fontIndices << fontIndex;
        }
    }
}

QVariantHash PsdTextDataConverter::styleToPSDStylesheet(const QMap<QString, QString> cssStyles,
                                 QVariantHash parentStyle, QTransform scaleToPx) {
    QVariantHash styleSheet = parentStyle;

    Q_FOREACH(QString key, cssStyles.keys()) {
        QString val = cssStyles.value(key);

        if (key == "font-size") {
            double size = val.toDouble();
            size = scaleToPx.map(QPointF(size, size)).x();
            styleSheet["/FontSize"] = size;
        } else if (key == "letter-spacing") {
            double space = val.toDouble();
            space = scaleToPx.map(QPointF(space, space)).x();
            double size = styleSheet["/FontSize"].toDouble();
            styleSheet["/Tracking"] = (space/size) * 1000.0;
        } else if (key == "line-height") {
            double space = val.toDouble();
            double size = styleSheet["/FontSize"].toDouble();
            styleSheet["/Leading"] = (space*size);
            styleSheet["/AutoLeading"] = false;
        } else if (key == "font-kerning") {
            if (val == "none") {
                styleSheet["/AutoKern"] = 0;
            }
        } else if (key == "baseline-shift") {
            if (val == "super") {
                styleSheet["/FontBaseline"] = 1;
            } else if (val == "super") {
                styleSheet["/FontBaseline"] = 2;
            } else {
                double offset = val.toDouble();
                offset = scaleToPx.map(QPointF(offset, offset)).y();
                styleSheet["/BaselineShift"] = offset;
            }
        } else if (key == "text-decoration") {
            QStringList decor = val.split(" ");
            Q_FOREACH(QString param, decor) {
                if (param == "underline") {
                    styleSheet["/UnderlinePosition"] = 1;
                    if (cssStyles.value("text-decoration-position").contains("right")) {
                        styleSheet["/UnderlinePosition"] = 2;
                    }
                } else if (param == "line-through"){
                    styleSheet["/StrikethroughPosition"] = 1;
                }
            }
        } else if (key == "font-variant") {
            QStringList params = val.split(" ");
            bool tab = params.contains("tabular-nums");
            bool old = params.contains("oldstyle-nums");
            Q_FOREACH(QString param, params) {
                if (param == "small-caps" || param == "all-small-caps") {
                    styleSheet["/FontCaps"] = 1;
                } else if (param == "titling-caps") {
                    styleSheet["/Titling"] = true;
                } else if (param == "no-common-ligatures"){
                    styleSheet["/Ligatures"] = false;
                } else if (param == "discretionary-ligatures"){
                    styleSheet["/DiscretionaryLigatures"] = true;
                } else if (param == "contextual"){
                    styleSheet["/ContextualLigatures"] = true;
                } else if (param == "diagonal-fractions"){
                    styleSheet["/Fractions"] = true;
                } else if (param == "ordinal"){
                    styleSheet["/Ordinals"] = true;
                } else if (param == "slashed-zero"){
                    styleSheet["/SlashedZero"] = true;
                } else if (param == "super") {
                    styleSheet["/FontOTPosition"] = 1;
                } else if (param == "sub") {
                    styleSheet["/FontOTPosition"] = 2;
                } else if (param == "ruby") {
                    styleSheet["/Ruby"] = true;
                } else if (param == "traditional") {
                    styleSheet["/JapaneseAlternateFeature"] = 1;
                } else if (param == "jis78") {
                    styleSheet["/JapaneseAlternateFeature"] = 3;
                }
            }
            styleSheet["/OldStyle"] = old;
            if (tab && old) {
                styleSheet["/FigureStyle"] = 4;
            } else if (tab) {
                styleSheet["/FigureStyle"] = 1;
            } else if (old) {
                styleSheet["/FigureStyle"] = 2;
            }
        } else if (key == "font-feature-settings") {
            QStringList params = val.split(",");
            Q_FOREACH(QString param, params) {
                if (param.trimmed() == "'swsh' 1") {
                    styleSheet["/Swash"] = true;
                } else if (param.trimmed() == "'titl' 1") {
                    styleSheet["/Titling"] = true;
                } else if (param.trimmed() == "'salt' 1") {
                    styleSheet["/StylisticAlternates"] = true;
                } else if (param.trimmed() == "'ornm' 1") {
                    styleSheet["/Ornaments"] = true;
                } else if (param.trimmed() == "'ital' 1") {
                    styleSheet["/Italics"] = true;
                } else if (param.trimmed() == "'numr' 1") {
                    styleSheet["/FontOTPosition"] = 3;
                } else if (param.trimmed() == "'dnum' 1") {
                    styleSheet["/FontOTPosition"] = 4;
                } else if (param.trimmed() == "'expt' 1") {
                    styleSheet["/JapaneseAlternateFeature"] = 2;
                } else if (param.trimmed() == "'hkna' 1") {
                    styleSheet["/Kana"] = true;
                } else if (param.trimmed() == "'palt' 1") {
                    styleSheet["/ProportionalMetrics"] = true;
                }
            }
        } else if (key == "text-orientation") {
            if (val == "upright") {
                styleSheet["/BaselineDirection"] = 1;
            } else if (val == "mixed") {
                styleSheet["/BaselineDirection"] = 2;
            }
        } else if (key == "text-combine-upright") {
            if (val == "all") {
                 styleSheet["/BaselineDirection"] = 3;
            }
        } else if (key == "word-break") {
            styleSheet["/NoBreak"] = val == "keep-all";
        } else if (key == "direction") {
            styleSheet["/DirOverride"] = val == "ltr"? 0 :1;
        } else if (key == "xml:lang") {
            if (psdLanguageMap.values().contains(val)) {
                styleSheet["/Language"] = psdLanguageMap.key(val);
            }
        } else if (key == "paint-order") {
            QStringList decor = val.split(" ");
            styleSheet["/FillFirst"] = decor.first() == "fill";
        } else {
            d->errors << "Unsupported css-style:" << key << val;
        }
    }

    return styleSheet;
}

void gatherFills(QDomElement el, QVariantHash &styleDict) {
    if (el.hasAttribute("fill")) {
        if (el.attribute("fill") != "none") {
            QColor c = QColor(el.attribute("fill"));
            styleDict["/FillFlag"] = true;
            styleDict["/FillColor"] = QVariantHash({ {"/StreamTag", "/SimplePaint"},
                                                     { "/Color", QVariantHash({
                                                           {"/Type", 1},
                                                           {"/Values", QVariantList({1.0, c.redF(), c.greenF(), c.blueF()})
                                                           }})}
                                                   });
        } else {
            styleDict["/FillFlag"] = false;
        }
    }
    if (el.hasAttribute("stroke")) {
        if (el.attribute("stroke") != "none" && el.attribute("stroke-width").toDouble() != 0) {
            QColor c = QColor(el.attribute("stroke"));
            styleDict["/StrokeFlag"] = true;
            styleDict["/StrokeColor"] = QVariantHash({ {"/StreamTag", "/SimplePaint"},
                                                       { "/Color", QVariantHash({
                                                             {"/Type", 1},
                                                             {"/Values", QVariantList({1.0, c.redF(), c.greenF(), c.blueF()})
                                                             }})}
                                                     });
        } else {
            styleDict["/StrokeFlag"] = false;
        }
    }
    if (el.hasAttribute("stroke-linejoin")) {
        QString val = el.attribute("stroke-linejoin");
        if (val == "miter") {
            styleDict["/LineJoin"] = 0;
        } else if (val == "round") {
            styleDict["/LineJoin"] = 1;
        } else if (val == "bevel") {
            styleDict["/LineJoin"] = 2;
        }
    }
    if (el.hasAttribute("stroke-linecap")) {
        QString val = el.attribute("stroke-linecap");
        if (val == "butt") {
            styleDict["/LineCap"] = 0;
        } else if (val == "round") {
            styleDict["/LineCap"] = 1;
        } else if (val == "square") {
            styleDict["/LineCap"] = 2;
        }
    }
    if (el.hasAttribute("stroke-width") && el.attribute("stroke-width").toDouble() != 0) {
        styleDict["/LineWidth"] = el.attribute("stroke-width").toDouble();
    }
}

void PsdTextDataConverter::gatherStyles(QDomElement el, QString &text,
                  QVariantHash parentStyle,
                  QMap<QString, QString> parentCssStyles,
                  QVariantList &styles,
                  QVariantList &fontSet, QTransform scaleToPx) {
    QMap<QString, QString> cssStyles = parentCssStyles;
    if (el.hasAttribute("style")) {
        QString style = el.attribute("style");
        QStringList dummy = style.split(";");

        Q_FOREACH(QString style, dummy) {
            QString key = style.split(":").first().trimmed();
            QString val = style.split(":").last().trimmed();
            cssStyles.insert(key, val);
        }
        Q_FOREACH(QString attribute, KoSvgTextProperties::supportedXmlAttributes()) {
            if (el.hasAttribute(attribute)) {
                cssStyles.insert(attribute, el.attribute(attribute));
            }
        }
    }

    if (el.firstChild().isText()) {
        QDomText textNode = el.firstChild().toText();
        QString currentText = textNode.data();
        text += currentText;

        QVariantHash styleDict = styleToPSDStylesheet(cssStyles, parentStyle, scaleToPx);
        gatherFills(el, styleDict);

        QVector<int> lengths;
        QVector<int> fontIndices;
        gatherFonts(cssStyles, currentText, fontSet, lengths, fontIndices);
        for (int i = 0; i< fontIndices.size(); i++) {
            QVariantHash curDict = styleDict;
            curDict["/Font"] = fontIndices.at(i);
            QVariantHash fDict = {
                {"/StyleSheet", QVariantHash({{"/Name", ""}, {"/Parent", 0}, {"/Features", curDict}})}
            };
            styles.append(QVariantHash({
                                           {"/Length", lengths.at(i)},
                                           {"/RunData", fDict},
                                       }));
        }

    } else if (el.childNodes().size()>0) {
        QVariantHash styleDict = styleToPSDStylesheet(cssStyles, parentStyle, scaleToPx);
        gatherFills(el, styleDict);
        QDomElement childEl = el.firstChildElement();
        while(!childEl.isNull()) {
            gatherStyles(childEl, text, styleDict, cssStyles, styles, fontSet, scaleToPx);
            childEl = childEl.nextSiblingElement();
        }
    }
}

QVariantHash PsdTextDataConverter::gatherParagraphStyle(QDomElement el,
                                 QVariantHash defaultProperties,
                                 bool &isHorizontal,
                                 QString *inlineSize,
                                 QTransform scaleToPx) {
    QString cssStyle = el.attribute("style");
    QStringList dummy = cssStyle.split(";");
    QMap<QString, QString> cssStyles;
    Q_FOREACH(QString style, dummy) {
        QString key = style.split(":").first().trimmed();
        QString val = style.split(":").last().trimmed();
        cssStyles.insert(key, val);
    }
    for (int i = 0; i < el.attributes().length(); i++) {
        const QDomAttr attr = el.attributes().item(i).toAttr();
        cssStyles.insert(attr.name(), attr.value());
    }
    int alignVal = 0;
    int anchorVal = 0;

    QVariantHash paragraphStyleSheet = defaultProperties;
    Q_FOREACH(QString key, cssStyles.keys()) {
        QString val = cssStyles.value(key);

        if (key == "text-align") {
            if (val == "start") {alignVal = 0;}
            if (val == "center") {alignVal = 2;}
            if (val == "end") {alignVal = 1;}
            if (val == "justify start") {alignVal = 3;}
            if (val == "justify center") {alignVal = 4;}
            if (val == "justify end") {alignVal = 5;}
            if (val == "justify") {alignVal = 6;}
        } else if (key == "text-anchor") {
            if (val == "start") {anchorVal = 0;}
            if (val == "middle") {anchorVal = 2;}
            if (val == "end") {anchorVal = 1;}
        } else if (key == "writing-mode") {
            if (val == "horizontal-tb") {
                isHorizontal = true;
            } else {
                isHorizontal = false;
            }
        } else if (key == "direction") {
            paragraphStyleSheet["/ParagraphDirection"] = val == "ltr"? 0 :1;
        } else if (key == "line-height") {
            paragraphStyleSheet["/AutoLeading"] = val.toDouble();
        } else if (key == "inline-size") {
            *inlineSize = val;
        }
    }
    if (cssStyles.keys().contains("shape-inside")) {
        paragraphStyleSheet["/Justification"] = alignVal;
    } else {
        paragraphStyleSheet["/Justification"] = anchorVal;
    }
    return QVariantHash{{"/Name", ""}, {"/Parent", 0}, {"/Features", paragraphStyleSheet}};
}

bool PsdTextDataConverter::convertToPSDTextEngineData(const QString &svgText, QRectF &boundingBox,
                                                               const QList<KoShape *> &shapesInside,
                                                               QVariantHash &txt2,
                                                               int &textIndex,
                                                               QString &textTotal,
                                                               bool &isHorizontal,
                                                               QTransform scaleToPx)
{
    QVariantHash root;

    QVariantHash model;
    QVariantHash view;

    QString text;
    QVariantList styles;
    QVariantList fontSet;

    QVariantList textObjects = txt2.value("/DocumentObjects").toHash().value("/TextObjects").toList();
    QVariantHash defaultParagraphProps = txt2.value("/DocumentObjects").toHash().value("/OriginalNormalParagraphFeatures").toHash();

    const int tIndex = textObjects.size();
    QVariantHash docResources = txt2.value("/DocumentResources").toHash();
    QVariantList textFrames = docResources.value("/TextFrameSet").toHash().value("/Resources").toList();
    const QVariantList resFontSet = docResources.value("/FontSet").toHash().value("/Resources").toList();

    Q_FOREACH(const QVariant entry, resFontSet) {
        const QVariantHash docFont = entry.toHash().value("/Resource").toHash();
        QVariantHash font = docFont.value("/Identifier").toHash();
        fontSet.append(font);
    }

    QVector<int> lengths;
    QVector<int> fontIndices;
    gatherFonts(KoSvgTextProperties::defaultProperties().convertToSvgTextAttributes(), "", fontSet, lengths, fontIndices);

    QDomDocument doc;
    doc.setContent(svgText);
    gatherStyles(doc.documentElement(), text, QVariantHash(), QMap<QString, QString>(), styles, fontSet, scaleToPx);

    QString inlineSize;
    QVariantHash paragraphStyle = gatherParagraphStyle(doc.documentElement(),
                                                      defaultParagraphProps,
                                                      isHorizontal, &inlineSize,
                                                      scaleToPx);

    text += '\n';
    model.insert("/Text", text);

    QVariantHash paragraphSet;
    paragraphSet.insert("/Length", QVariant(text.length()));
    paragraphSet.insert("/RunData", QVariantHash{{"/ParagraphSheet", paragraphStyle}});

    model.insert("/ParagraphRun", QVariantHash{{"/RunArray", QVariantList({paragraphSet})}});

    QVariantHash styleRun;
    QVariantList properStyleRun;
    Q_FOREACH(QVariant entry, styles) {
        properStyleRun.append(entry);
    }
    styleRun.insert("/RunArray", properStyleRun);

    model.insert("/StyleRun", styleRun);

    QVariantHash storySheet;
    storySheet.insert("/UseFractionalGlyphWidths", true);
    storySheet.insert("/AntiAlias", 1);
    model.insert("/StorySheet", storySheet);

    QRectF bounds;
    if (!(inlineSize.isEmpty() || inlineSize == "auto")) {
        bounds = boundingBox;
        bool ok;
        double inlineSizeVal = inlineSize.toDouble(&ok);
        if (ok) {
            if (isHorizontal) {
                bounds.setWidth(inlineSizeVal);
            } else {
                bounds.setHeight(inlineSizeVal);
            }
        }
    } else {
        bounds = QRectF();
    }

    int shapeType = bounds.isEmpty()? 0: 1;
    int writingDirection = isHorizontal? 0: 2;

    const int textFrameIndex = textFrames.size();
    QVariantHash newTextFrame;
    QVariantHash newTextFrameData;
    newTextFrameData.insert("/LineOrientation", writingDirection);

    QList<QPointF> points;

    QScopedPointer<KoPathShape> textShape;
    Q_FOREACH(KoShape *shape, shapesInside) {
        KoPathShape *p = dynamic_cast<KoPathShape*>(shape);
        if (p) {
            textShape.reset(p);
            break;
        }
    }
    if (textShape) {
        for (int i = 0; i<textShape->subpathPointCount(0); i++) {
            KoPathSegment s = textShape->segmentByIndex(KoPathPointIndex(0, i));
            points.append(s.first()->point());
            points.append(s.first()->controlPoint2());
            points.append(s.second()->controlPoint1());
            points.append(s.second()->point());
        }
    } else if (!bounds.isEmpty()) {
        points.append(bounds.topLeft());
        points.append(bounds.topLeft());
        points.append(bounds.topRight());
        points.append(bounds.topRight());
        points.append(bounds.topRight());
        points.append(bounds.topRight());
        points.append(bounds.bottomRight());
        points.append(bounds.bottomRight());
        points.append(bounds.bottomRight());
        points.append(bounds.bottomRight());
        points.append(bounds.bottomLeft());
        points.append(bounds.bottomLeft());
        points.append(bounds.bottomLeft());
        points.append(bounds.bottomLeft());
        points.append(bounds.topLeft());
        points.append(bounds.topLeft());
    }
    if (!points.isEmpty()) {
        QVariantList p;
        for(int i = 0; i < points.size(); i++) {
            QPointF p2 = scaleToPx.map(points.at(i));
            p.append(p2.x());
            p.append(p2.y());
        }
        newTextFrame.insert("/Bezier", QVariantHash({{"/Points", p}}));
        shapeType = 1;
    }
    newTextFrameData.insert("/Type", shapeType);
    newTextFrame.insert("/Data", newTextFrameData);

    view.insert("/Frames", QVariantList({QVariantHash({{"/Resource", textFrameIndex}})}));

    QVariantList bbox = {0.0, 0.0, bounds.width(), bounds.height()};
    QVariantList bbox2 = {bounds.left(), bounds.top(), bounds.right(), bounds.bottom()};

    QVariantHash rendered {
        {"/RunData", QVariantHash({{"/LineCount", 1}})},
        {"/Length", textTotal.length()}
    };
    view.insert("/RenderedData", QVariantHash({{"/RunArray", QVariantList({rendered})}}));

    textFrames.append(QVariantHash({{"/Resource", newTextFrame}}));
    textObjects.append(QVariantHash({{"/Model", model}, {"/View", view}}));

    textTotal = text;

    QVariantList newFontSet;

    Q_FOREACH(const QVariant entry, fontSet) {
        newFontSet.append(QVariantHash({{"/Resource", QVariantHash({{"/StreamTag", "/CoolTypeFont"}, {"/Identifier", entry}})}}));
    }

    QVariantHash docObjects = txt2.value("/DocumentObjects").toHash();
    docObjects.insert("/TextObjects", textObjects);
    txt2.insert("/DocumentObjects", docObjects);
    docResources.insert("/TextFrameSet", QVariantHash({{"/Resources", textFrames}}));
    docResources.insert("/FontSet", QVariantHash({{"/Resources", newFontSet}}));
    txt2.insert("/DocumentResources", docResources);
    textIndex = tIndex;

    return true;
}

QStringList PsdTextDataConverter::errors() const
{
    return d->errors;
}

QStringList PsdTextDataConverter::warnings() const
{
    return d->warnings;
}
