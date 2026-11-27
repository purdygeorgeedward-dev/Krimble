/*
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_HISTOGRAM_PRODUCER_
#define _KO_HISTOGRAM_PRODUCER_

#include <QtGlobal>

#include "kritapigment_export.h"

#include <KoGenericRegistry.h>
#include <KoID.h>

class QString;
class KoChannelInfo;
class KoColorSpace;

class KRITAPIGMENT_EXPORT KoHistogramProducer
{
public:
    KoHistogramProducer() : m_skipTransparent(true), m_skipUnselected(true) {}
    virtual ~KoHistogramProducer() {}

    virtual void clear() = 0;

    virtual void addRegionToBin(const quint8 * pixels, const quint8 * selectionMask, quint32 nPixels, const KoColorSpace* colorSpace) = 0;

    virtual void setView(qreal from, qreal width) = 0;
    virtual void setSkipTransparent(bool set) {
        m_skipTransparent = set;
    }
    virtual void setSkipUnselected(bool set) {
        m_skipUnselected = set;
    }

    virtual const KoID& id() const = 0;
    virtual QList<KoChannelInfo *> channels() = 0;
    virtual qint32 numberOfBins() = 0;
    virtual QString positionToString(qreal pos) const = 0;
    virtual qreal viewFrom() const = 0;
    virtual qreal viewWidth() const = 0;
    virtual qreal maximalZoom() const = 0;

    virtual qint32 count() = 0;
    virtual qint32 getBinAt(qint32 channel, qint32 position) = 0;
    virtual qint32 outOfViewLeft(qint32 channel) = 0;
    virtual qint32 outOfViewRight(qint32 channel) = 0;
protected:
    bool m_skipTransparent;
    bool m_skipUnselected;
};

class KRITAPIGMENT_EXPORT KoHistogramProducerFactory
{
public:
    explicit KoHistogramProducerFactory(const KoID &id) : m_id(id) {}
    virtual ~KoHistogramProducerFactory() {}

    virtual KoHistogramProducer *generate() = 0;

    virtual bool isCompatibleWith(const KoColorSpace* colorSpace, bool strict = false) const = 0;

    virtual float preferrednessLevelWith(const KoColorSpace* colorSpace) const = 0;

    virtual QString id() const {
        return m_id.id();
    }

    virtual QString name() const {
        return m_id.name();
    }
protected:
    KoID m_id;
};

class KRITAPIGMENT_EXPORT KoHistogramProducerFactoryRegistry
        : public KoGenericRegistry<KoHistogramProducerFactory*>
{
public:
    KoHistogramProducerFactoryRegistry();
    ~KoHistogramProducerFactoryRegistry() override;
    static KoHistogramProducerFactoryRegistry* instance();
    QList<QString> keysCompatibleWith(const KoColorSpace* colorSpace, bool isStrict=false) const;

private:
    KoHistogramProducerFactoryRegistry(const KoHistogramProducerFactoryRegistry&);
    KoHistogramProducerFactoryRegistry operator=(const KoHistogramProducerFactoryRegistry&);
};

#endif
