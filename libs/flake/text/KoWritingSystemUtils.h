/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOWRITINGSYSTEMUTILS_H
#define KOWRITINGSYSTEMUTILS_H

#include <QFontDatabase>
#include <QLocale>
#include <kritaflake_export.h>

class KRITAFLAKE_EXPORT KoWritingSystemUtils
{
public:
    static QString scriptTagForWritingSystem(QFontDatabase::WritingSystem system);
    static QFontDatabase::WritingSystem writingSystemForScriptTag(const QString &tag);

    static QString scriptTagForQLocaleScript(QLocale::Script script);
    static QLocale::Script scriptForScriptTag(const QString &tag);

    static QString scriptTagForQCharScript(QChar::Script script);
    static QChar::Script qCharScriptForScriptTag(const QString &tag);

    static QMap<QString, QString> samples();

    static QString sampleTagForQLocale(const QLocale &locale);

    struct KRITAFLAKE_EXPORT Bcp47Locale {
        QStringList languageTags;
        QString scriptTag;
        QString regionTag;
        QStringList variantTags;
        QStringList extensionTags;
        QStringList privateUseTags;

        bool isValid() const;
        QString toPosixLocaleFormat() const;
        QString toString() const;
    };

    static Bcp47Locale parseBcp47Locale(const QString &locale);

    static QLocale localeFromBcp47Locale(const Bcp47Locale &locale);

    static QLocale localeFromBcp47Locale(const QString &locale);
};

#endif
