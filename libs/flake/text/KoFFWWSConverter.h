/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOFFWWSCONVERTER_H
#define KOFFWWSCONVERTER_H

#include <QHash>
#include <KoFontLibraryResourceUtils.h>
#include "KoCSSFontInfo.h"
#include <kritaflake_export.h>

#include <KoSvgText.h>
#include <QLocale>
#include <QDateTime>

#include <optional>

struct KoFontFamilyWWSRepresentation {
    QString fontFamilyName;
    QString typographicFamilyName;

    QHash<QLocale, QString> localizedFontFamilyNames;
    QHash<QLocale, QString> localizedTypographicFamily;
    QHash<QLocale, QString> localizedTypographicStyles;

    QDateTime lastModified;

    QHash<QString, QString> sampleStrings;
    QList<QLocale> supportedLanguages;

    QHash<QString, KoSvgText::FontFamilyAxis> axes;
    QList<KoSvgText::FontFamilyStyleInfo> styles;

    KoSvgText::FontFormatType type = KoSvgText::UnknownFontType;
    bool isVariable = false;
    bool colorClrV0 = false;
    bool colorClrV1 = false;
    bool colorSVG = false;
    bool colorBitMap = false;
};

class KRITAFLAKE_EXPORT KoFFWWSConverter
{
public:
    KoFFWWSConverter();
    ~KoFFWWSConverter();

    struct FontFileEntry {
        QString fileName;
        int fontIndex;
    };

    bool addFontFromPattern(const FcPattern *pattern, FT_LibrarySP freeTypeLibrary);

    bool addFontFromFile(const QString &filename, const int index, FT_LibrarySP freeTypeLibrary);

    void addSupportedLanguagesByFile(const QString &filename, const int index, const QList<QLocale> &supportedLanguages, FcCharSet *set);

    void sortIntoWWSFamilies();

    void addGenericFamily(const QString &name);

    QList<KoFontFamilyWWSRepresentation> collectFamilies() const;

    std::optional<KoFontFamilyWWSRepresentation> representationByFamilyName(const QString &familyName) const;

    std::optional<QString> wwsNameByFamilyName(const QString familyName) const;

    QVector<FontFileEntry> candidatesForCssValues(const KoCSSFontInfo info,
                                       quint32 xRes = 72,
                                       quint32 yRes = 72) const;

    void debugInfo() const;

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
