/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <QString>
#include <QStringList>
#include <QVector>
#include <QScopedPointer>
#include <kritaflake_export.h>

struct KoOpenTypeFeatureInfo
{
    enum OpenTypeTable {
        GSUB1,
        GSUB2,
        GSUB3,
        GSUB4,
        GSUB5,
        GSUB6,
        GSUB7,
        GSUB8,
        GPOS1,
        GPOS2,
        GPOS3,
        GPOS4,
        GPOS5,
        GPOS6,
        GPOS7,
        GPOS8,
        GPOS9,
    };
    KoOpenTypeFeatureInfo() {}
    KoOpenTypeFeatureInfo(const QByteArray &tag,
                          const QString &name,
                          const QString &description,
                          const QVector<OpenTypeTable> &tables,
                          const bool glyphPalette = false,
                          const int maxValue = 1)
    : tag(tag)
    , name(name)
    , description(description)
    , tables(tables)
    , glyphPalette(glyphPalette)
    , maxValue(maxValue)
    {
    }

    QByteArray tag;

    QString name;

    QString description;

    QString sample;

    QStringList namedParameters;

    QVector<OpenTypeTable> tables;

    bool glyphPalette{false};

    int maxValue;

};

class KRITAFLAKE_EXPORT KoOpenTypeFeatureInfoFactory
{
public:
    KoOpenTypeFeatureInfoFactory();
    ~KoOpenTypeFeatureInfoFactory();

    KoOpenTypeFeatureInfo infoByTag(const QByteArray &tag) const;

    QList<QString> tags() const;

private:
    struct Private;

    QScopedPointer<Private> d;
};
