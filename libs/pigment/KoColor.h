/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOCOLOR_H
#define KOCOLOR_H

#include <QColor>
#include <QMetaType>
#include <QtGlobal>
#include "kritapigment_export.h"
#include "KoColorConversionTransformation.h"
#include "KoColorSpaceRegistry.h"
#include "KoColorSpaceTraits.h"
#include <boost/operators.hpp>

class QDomDocument;
class QDomElement;

class KoColorProfile;
class KoColorSpace;

class KRITAPIGMENT_EXPORT KoColor : public boost::equality_comparable<KoColor>
{

public:
    KoColor();

    explicit KoColor(const KoColorSpace * colorSpace);

    KoColor(const QColor & color, const KoColorSpace * colorSpace);

    KoColor(const quint8 * data, const KoColorSpace * colorSpace);

    KoColor(const KoColor &src, const KoColorSpace * colorSpace);

    KoColor(const KoColor &rhs)
        : m_colorSpace(rhs.m_colorSpace)
        , m_size(rhs.m_size)
        , m_metadata(rhs.m_metadata)
    {
        memcpy(m_data, rhs.m_data, m_size);
    }

    inline KoColor &operator=(const KoColor &rhs) {
        if (&rhs == this) {
            return *this;
        }

        m_colorSpace = rhs.m_colorSpace;
        m_metadata = rhs.m_metadata;
        m_size = rhs.m_size;
        memcpy(m_data, rhs.m_data, m_size);

        assertPermanentColorspace();

        return *this;
    }

    bool operator==(const KoColor &other) const;

    const KoColorSpace * colorSpace() const {
        return m_colorSpace;
    }

    const KoColorProfile *profile() const;

    void convertTo(const KoColorSpace * cs,
                   KoColorConversionTransformation::Intent renderingIntent,
                   KoColorConversionTransformation::ConversionFlags conversionFlags);

    void convertTo(const KoColorSpace * cs);

    KoColor convertedTo(const KoColorSpace * cs,
                        KoColorConversionTransformation::Intent renderingIntent,
                        KoColorConversionTransformation::ConversionFlags conversionFlags) const;

    KoColor convertedTo(const KoColorSpace * cs) const;

    void setProfile(const KoColorProfile *profile);

    void setColor(const quint8 * data, const KoColorSpace * colorSpace = 0);

    void fromKoColor(const KoColor& src);

    void toQColor(QColor *c) const;
    QColor toQColor() const;

    void setOpacity(quint8 alpha);
    void setOpacity(qreal alpha);
    quint8 opacityU8() const;
    qreal opacityF() const;

    void fromQColor(const QColor& c);

    quint8 * data() {
        return m_data;
    }

    const quint8 * data() const {
        return m_data;
    }

    void subtract(const KoColor &value);

    KoColor subtracted(const KoColor &value) const;

    void add(const KoColor &value);

    KoColor added(const KoColor &value) const;

    void toXML(QDomDocument& doc, QDomElement& colorElt) const;

    static KoColor fromXML(const QDomElement& elt, const QString & channelDepthId);

    static KoColor fromXML(const QDomElement& elt, const QString & channelDepthId, bool* ok);

    QString toXML() const;

    static KoColor fromXML(const QString &xml);

    QString toSVG11(QHash<QString, const KoColorProfile *> *profileList) const;

    static KoColor fromSVG11(const QString value, QHash<QString, const KoColorProfile*> profileList, KoColor current = KoColor());

    static QString toQString(const KoColor &color);

    void addMetadata(QString key, QVariant value);

    QMap<QString, QVariant> metadata() const;

    void clearMetadata();

    static KoColor createTransparent(const KoColorSpace *cs);

#ifndef NODEBUG
    void dump() const;
#endif

private:
    inline void assertPermanentColorspace() {
#ifndef NODEBUG
        if (m_colorSpace) {
            Q_ASSERT(*m_colorSpace == *KoColorSpaceRegistry::instance()->permanentColorspace(m_colorSpace));
        }
#endif
    }

    const KoColorSpace *m_colorSpace;
    quint8 m_data[MAX_PIXEL_SIZE];
    quint8 m_size;
    QMap<QString, QVariant> m_metadata;
};

Q_DECLARE_METATYPE(KoColor)

KRITAPIGMENT_EXPORT QDebug operator<<(QDebug dbg, const KoColor &color);

#endif
