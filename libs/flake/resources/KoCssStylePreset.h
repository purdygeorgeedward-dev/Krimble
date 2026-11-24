/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOCSSSTYLEPRESET_H
#define KOCSSSTYLEPRESET_H

#include <KoResource.h>
#include <KoSvgTextProperties.h>
#include <kritaflake_export.h>

class KoCssStylePreset;
typedef QSharedPointer<KoCssStylePreset> KoCssStylePresetSP;

class KRITAFLAKE_EXPORT KoCssStylePreset : public KoResource
{
public:
    KoCssStylePreset(const QString &filename);
    KoCssStylePreset(const KoCssStylePreset &rhs);
    KoCssStylePreset &operator=(const KoCssStylePreset &rhs) = delete;
    ~KoCssStylePreset();

    KoSvgTextProperties properties(int ppi = 72, bool removeKraProps = false) const;

    void setProperties(const KoSvgTextProperties &properties);

    QString description() const;
    void setDescription(const QString &desc);

    QString styleType() const;
    void setStyleType(const QString &type);

    QString sampleText() const;

    void setSampleText(const QString &text);

    QString beforeText() const;

    void setBeforeText(const QString &text);

    QString afterText() const;

    void setAfterText(const QString &text);

    QString sampleSvg() const;

    QSizeF paragraphSampleSize() const;
    void setParagraphSampleSize(const QSizeF size);

    int storedPPIResolution() const;

    void setStoredPPIResolution(const int ppi);

    KoShape* generateSampleShape() const;

    Qt::Alignment alignSample() const;

    QString primaryFontFamily() const;

    void updateAlignSample();

public:
    KoResourceSP clone() const override;
    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice *dev) const override;
    QString defaultFileExtension() const override;
    void updateThumbnail() override;
    QPair<QString, QString> resourceType() const override;
private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
