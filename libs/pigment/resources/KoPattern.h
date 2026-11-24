/*
    SPDX-FileCopyrightText: 2000 Matthias Elter <elter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOPATTERN_H
#define KOPATTERN_H

#include <KoResource.h>
#include <kritapigment_export.h>

#include <QMetaType>
#include <QSharedPointer>

class KoPattern;
typedef QSharedPointer<KoPattern> KoPatternSP;

class KRITAPIGMENT_EXPORT KoPattern : public KoResource
{
public:

    explicit KoPattern(const QString &filename);

    KoPattern(const QImage &image, const QString &name, const QString &filename);
    ~KoPattern() override;

    KoPattern(const KoPattern &rhs);
    KoPattern& operator=(const KoPattern& rhs) = delete;
    KoResourceSP clone() const override;

public:

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice* dev) const override;

    bool loadPatFromDevice(QIODevice *dev);
    bool savePatToDevice(QIODevice* dev) const;

    qint32 width() const;
    qint32 height() const;

    QString defaultFileExtension() const override;

    QPair<QString, QString> resourceType() const override {
        return QPair<QString, QString>(ResourceType::Patterns, "");
    }

    QImage pattern() const;

    bool hasAlpha() const;

    KoPatternSP cloneWithoutAlpha() const;

private:

    void setPatternImage(const QImage& image);
    void checkForAlpha(const QImage& image);

private:
    QImage m_pattern;
    bool m_hasAlpha = false;
};

Q_DECLARE_METATYPE(KoPattern*)

Q_DECLARE_METATYPE(QSharedPointer<KoPattern>)

#endif

