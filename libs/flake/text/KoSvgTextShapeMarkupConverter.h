/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOSVGTEXTSHAPEMARKUPCONVERTER_H
#define KOSVGTEXTSHAPEMARKUPCONVERTER_H

#include "kritaflake_export.h"

#include <QScopedPointer>
#include <QTextDocument>
#include <QTextCharFormat>

#include <optional>

class QRectF;
class KoSvgTextShape;
class KoShape;
class KoColorSpace;

class KRITAFLAKE_EXPORT KoSvgTextShapeMarkupConverter
{
public:
    KoSvgTextShapeMarkupConverter(KoSvgTextShape *shape);
    ~KoSvgTextShapeMarkupConverter();

    bool convertToSvg(QString *svgText, QString *stylesText);

    bool convertFromSvg(const QString &svgText, const QString &stylesText, const QRectF &boundsInPixels, qreal pixelsPerInch);

    bool convertToHtml(QString *htmlText);

    bool convertFromHtml(const QString &htmlText, QString *svgText, QString *styles);

    bool convertDocumentToSvg(const QTextDocument *doc, QString *svgText);

    bool convertSvgToDocument(const QString &svgText, QTextDocument *doc);

    QStringList errors() const;
    QStringList warnings() const;

    QString style(QTextCharFormat format, QTextBlockFormat blockFormat, QTextCharFormat mostCommon = QTextCharFormat(), bool includeLineHeight = false);

    struct ExtraStyles;

    static QVector<QTextFormat> stylesFromString(QStringList styles, QTextCharFormat currentCharFormat, QTextBlockFormat currentBlockFormat, ExtraStyles &extraStyles);
    QTextFormat formatDifference(QTextFormat test, QTextFormat reference);

private:
    struct Private;
    const QScopedPointer<Private> d;

public:

    enum class WrappingMode {
        QtLegacy = 0,
        WhiteSpacePre,
        WhiteSpacePreWrap,
    };

    static constexpr QTextFormat::Property WrappingModeProperty =
        static_cast<QTextFormat::Property>(QTextFormat::UserProperty + 56784);
    static constexpr QTextFormat::Property InlineSizeProperty =
        static_cast<QTextFormat::Property>(WrappingModeProperty + 1);

    static WrappingMode getWrappingMode(const QTextFrameFormat &frameFormat);
    static void setWrappingMode(QTextFrameFormat *frameFormat, WrappingMode wrappingMode);

    static std::optional<double> getInlineSize(const QTextFrameFormat &frameFormat);
    static void setInlineSize(QTextFrameFormat *frameFormat, double inlineSize);
};

#endif
