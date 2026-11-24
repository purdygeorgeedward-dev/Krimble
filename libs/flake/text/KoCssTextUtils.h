/*
 *  SPDX-FileCopyrightText: 2022 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOCSSTEXTUTILS_H
#define KOCSSTEXTUTILS_H

#include <KoSvgText.h>
#include <QDebug>
#include <QLocale>
#include <QString>

#include "kritaflake_export.h"
class KRITAFLAKE_EXPORT KoCssTextUtils
{
public:
    static QString transformTextToUpperCase(const QString &text, const QString &langCode, QVector<QPair<int, int>> &positions);

    static QString transformTextToLowerCase(const QString &text, const QString &langCode, QVector<QPair<int, int>> &positions);

    static QString transformTextCapitalize(const QString &text, QString langCode, QVector<QPair<int, int>> &positions);

    static QString transformTextFullWidth(const QString &text);
    static QString transformTextFullSizeKana(const QString &text);

    static QVector<bool> collapseSpaces(QString *text, QMap<int, KoSvgText::TextSpaceCollapse> collapseMethods);

    static bool collapseLastSpace(QChar c, KoSvgText::TextSpaceCollapse collapseMethod);

    static bool hangLastSpace(const QChar c,
                              KoSvgText::TextSpaceCollapse collapseMethod,
                              KoSvgText::TextWrap wrapMethod,
                              bool &force, bool nextCharIsHardBreak);

    static bool characterCanHang(QChar c, KoSvgText::HangingPunctuations hangType);

    static bool IsCssWordSeparator(QString grapheme);

    static QStringList textToUnicodeGraphemeClusters(const QString &text, const QString &langCode);

    static QVector<QPair<bool, bool>> justificationOpportunities(QString text, QString langCode);

    static QString getBidiOpening(bool ltr, KoSvgText::UnicodeBidi bidi);

    static QString getBidiClosing(KoSvgText::UnicodeBidi bidi);

    static void removeText(QString &text, int &start, int length);

    static qreal cssSelectFontStyleValue(const QVector<qreal> &values,
                                         const qreal targetValue,
                                         const qreal defaultValue,
                                         const qreal defaultValueUpper,
                                         const bool shouldNotReturnDefault);
};

#endif
