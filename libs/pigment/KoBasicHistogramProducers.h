/*
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _Ko_BASIC_HISTOGRAM_PRODUCERS_
#define _Ko_BASIC_HISTOGRAM_PRODUCERS_

#include "KoHistogramProducer.h"

#include <QVector>

#include <KoConfig.h>

#include "KoColorSpace.h"
#include "KoID.h"
#include "kritapigment_export.h"
#include "KoColorSpaceRegistry.h"

class KRITAPIGMENT_EXPORT KoBasicHistogramProducer : public KoHistogramProducer
{
public:
    explicit KoBasicHistogramProducer(const KoID& id, int channelCount, int nrOfBins);
    explicit KoBasicHistogramProducer(const KoID& id, int nrOfBins, const KoColorSpace *colorSpace);
    ~KoBasicHistogramProducer() override {}

    void clear() override;

    void setView(qreal from, qreal size) override {
        m_from = from; m_width = size;
    }

    const KoID& id() const override {
        return m_id;
    }
    QList<KoChannelInfo *> channels() override {
        return m_colorSpace->channels();
    }
    qint32 numberOfBins() override {
        return m_nrOfBins;
    }
    qreal viewFrom() const override {
        return m_from;
    }
    qreal viewWidth() const override {
        return m_width;
    }

    qint32 count() override {
        return m_count;
    }

    qint32 getBinAt(int channel, int position) override {
        return m_bins.at(externalToInternal(channel)).at(position);
    }

    qint32 outOfViewLeft(int channel) override {
        return m_outLeft.at(externalToInternal(channel));
    }

    qint32 outOfViewRight(int channel) override {
        return m_outRight.at(externalToInternal(channel));
    }

protected:
    virtual int externalToInternal(int ext) {
        if (channels().count() > 0 && m_external.count() == 0)
            makeExternalToInternal();
        return m_external.at(ext);
    }
    void makeExternalToInternal();
    typedef QVector<quint32> vBins;
    QVector<vBins> m_bins;
    vBins m_outLeft, m_outRight;
    qreal m_from, m_width;
    qint32 m_count;
    int m_channels, m_nrOfBins;
    const KoColorSpace *m_colorSpace;
    KoID m_id;
    QVector<qint32> m_external;
};

class KRITAPIGMENT_EXPORT KoBasicU8HistogramProducer : public KoBasicHistogramProducer
{
public:
    KoBasicU8HistogramProducer(const KoID& id, const KoColorSpace *colorSpace);
    ~KoBasicU8HistogramProducer() override {}
    void addRegionToBin(const quint8 * pixels, const quint8 * selectionMask, quint32 nPixels, const KoColorSpace *colorSpace) override;
    QString positionToString(qreal pos) const override;
    qreal maximalZoom() const override {
        return 1.0;
    }
};

class KRITAPIGMENT_EXPORT KoBasicU16HistogramProducer : public KoBasicHistogramProducer
{
public:
    KoBasicU16HistogramProducer(const KoID& id, const KoColorSpace *colorSpace);
    ~KoBasicU16HistogramProducer() override {}
    void addRegionToBin(const quint8 * pixels, const quint8 * selectionMask, quint32 nPixels, const KoColorSpace *colorSpace) override;
    QString positionToString(qreal pos) const override;
    qreal maximalZoom() const override;
};

class KRITAPIGMENT_EXPORT KoBasicF32HistogramProducer : public KoBasicHistogramProducer
{
public:
    KoBasicF32HistogramProducer(const KoID& id, const KoColorSpace *colorSpace);
    ~KoBasicF32HistogramProducer() override {}
    void addRegionToBin(const quint8 * pixels, const quint8 * selectionMask, quint32 nPixels, const KoColorSpace *colorSpace) override;
    QString positionToString(qreal pos) const override;
    qreal maximalZoom() const override;
};

#ifdef HAVE_OPENEXR
class KRITAPIGMENT_EXPORT KoBasicF16HalfHistogramProducer : public KoBasicHistogramProducer
{
public:
    KoBasicF16HalfHistogramProducer(const KoID& id, const KoColorSpace *colorSpace);
    ~KoBasicF16HalfHistogramProducer() override {}
    void addRegionToBin(const quint8 * pixels, const quint8 * selectionMask, quint32 nPixels, const KoColorSpace *colorSpace) override;
    QString positionToString(qreal pos) const override;
    qreal maximalZoom() const override;
};
#endif

template<class T> class KoBasicHistogramProducerFactory : public KoHistogramProducerFactory
{
public:
    KoBasicHistogramProducerFactory(const KoID& id, const QString& modelId, const QString& depthId )
            : KoHistogramProducerFactory(id), m_modelId(modelId), m_depthId(depthId) {
    }
    ~KoBasicHistogramProducerFactory() override {}

    KoHistogramProducer *generate() override {
        KoHistogramProducer *producer = 0;
        const KoColorSpace *cs = KoColorSpaceRegistry::instance()->colorSpace(m_modelId, m_depthId, 0);
        if (cs) {
            producer = new T(KoID(id(), name()), cs);
        }
        return producer;

    }
    bool isCompatibleWith(const KoColorSpace* colorSpace, bool strict = false) const override {
        if( strict ){
            return colorSpace->colorDepthId().id() == m_depthId;
        }
        return colorSpace->colorModelId().id() == m_modelId || colorSpace->colorDepthId().id() == m_depthId;
    }
    float preferrednessLevelWith(const KoColorSpace* colorSpace) const override {
        return 0.5 * ( (colorSpace->colorModelId().id() == m_modelId) + (colorSpace->colorDepthId().id() == m_depthId) );
    }
protected:
    QString m_modelId, m_depthId;
};

class KRITAPIGMENT_EXPORT KoGenericRGBHistogramProducer : public KoBasicHistogramProducer
{
public:
    KoGenericRGBHistogramProducer();
    ~KoGenericRGBHistogramProducer() override {}
    void addRegionToBin(const quint8 * pixels, const quint8 * selectionMask, quint32 nPixels, const KoColorSpace *colorSpace) override;
    QString positionToString(qreal pos) const override;
    qreal maximalZoom() const override;
    QList<KoChannelInfo *> channels() override;
protected:
    QList<KoChannelInfo *> m_channelsList;
};

class  KRITAPIGMENT_EXPORT KoGenericRGBHistogramProducerFactory : public KoHistogramProducerFactory
{
public:
    KoGenericRGBHistogramProducerFactory();

    ~KoGenericRGBHistogramProducerFactory() override {}

    KoHistogramProducer *generate() override {
        return new KoGenericRGBHistogramProducer();
    }

    bool isCompatibleWith(const KoColorSpace*, bool strict = false) const override {
        Q_UNUSED(strict);
        return true;
    }

    float preferrednessLevelWith(const KoColorSpace*) const override {
        return 0.0;
    }
};

class  KRITAPIGMENT_EXPORT KoGenericLabHistogramProducer : public KoBasicHistogramProducer
{
public:
    KoGenericLabHistogramProducer();
    ~KoGenericLabHistogramProducer() override;
    void addRegionToBin(const quint8 * pixels, const quint8 * selectionMask, quint32 nPixels, const KoColorSpace *colorSpace) override;
    QString positionToString(qreal pos) const override;
    qreal maximalZoom() const override;
    QList<KoChannelInfo *> channels() override;
protected:
    QList<KoChannelInfo *> m_channelsList;
};

class KoGenericLabHistogramProducerFactory : public KoHistogramProducerFactory
{
public:
    KoGenericLabHistogramProducerFactory();
    ~KoGenericLabHistogramProducerFactory() override {}

    KoHistogramProducer *generate() override {
        return new KoGenericLabHistogramProducer();
    }

    bool isCompatibleWith(const KoColorSpace*, bool strict = false) const override {
        Q_UNUSED(strict);
        return true;
    }

    float preferrednessLevelWith(const KoColorSpace*) const override {
        return 0.0;
    }
};

#endif
