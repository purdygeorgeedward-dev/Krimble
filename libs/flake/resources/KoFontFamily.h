/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOFONTFAMILY_H
#define KOFONTFAMILY_H

#include <KoResource.h>
#include <KoFFWWSConverter.h>

class KoFontFamily;
typedef QSharedPointer<KoFontFamily> KoFontFamilySP;

class KRITAFLAKE_EXPORT KoFontFamily : public KoResource
{
public:
    explicit KoFontFamily(KoFontFamilyWWSRepresentation representation);

    KoFontFamily(const QString &filename);
    ~KoFontFamily();

    KoFontFamily(const KoFontFamily &rhs);
    KoFontFamily &operator=(const KoFontFamily &rhs) = delete;
    KoResourceSP clone() const override;

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;

    bool isSerializable() const override;
    QPair<QString, QString> resourceType() const override;

    void updateThumbnail() override;

    QString typographicFamily() const;

    QString translatedFontName(QStringList locales) const;

    bool isVariable() const;
    bool colorBitmap() const;
    bool colorClrV0() const;
    bool colorClrV1() const;
    bool colorSVG() const;

    QList<KoSvgText::FontFamilyAxis> axes() const;
    QList<KoSvgText::FontFamilyStyleInfo> styles() const;

    QDateTime lastModified() const;

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
