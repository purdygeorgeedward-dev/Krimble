/*
 *  SPDX-FileCopyrightText: 2002 patrick julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_PAINT_DEVICE_IMPL_H_
#define KIS_PAINT_DEVICE_IMPL_H_

#include <QObject>
#include <QRect>
#include <QVector>

#include "kis_debug.h"

#include <KoColorConversionTransformation.h>

#include "kis_types.h"
#include "kis_shared.h"
#include "kis_default_bounds_base.h"

#include <kritaimage_export.h>

class KUndo2Command;
class QRect;
class QImage;
class QPoint;
class QString;
class QColor;
class QIODevice;

class KoColor;
class KoColorSpace;
class KoColorProfile;

class KisRegion;
class KisDataManager;
class KisPaintDeviceWriter;
class KisKeyframe;
class KisRasterKeyframeChannel;

class KisPaintDeviceFramesInterface;

class KisInterstrokeData;
using KisInterstrokeDataSP = QSharedPointer<KisInterstrokeData>;

typedef KisSharedPtr<KisDataManager> KisDataManagerSP;

namespace KritaUtils {
enum DeviceCopyMode {
    CopySnapshot = 0,
    CopyAllFrames
};
}

class KRITAIMAGE_EXPORT KisPaintDevice
    : public QObject
    , public KisShared
{

    Q_OBJECT

public:

    explicit KisPaintDevice(const KoColorSpace * colorSpace, const QString& name = QString());

    KisPaintDevice(KisNodeWSP parent, const KoColorSpace * colorSpace, KisDefaultBoundsBaseSP defaultBounds = KisDefaultBoundsBaseSP(), const QString& name = QString());

    KisPaintDevice(const KisPaintDevice& rhs, KritaUtils::DeviceCopyMode copyMode = KritaUtils::CopySnapshot, KisNode *newParentNode = 0);
    ~KisPaintDevice() override;

    void makeFullCopyFrom(const KisPaintDevice& rhs, KritaUtils::DeviceCopyMode copyMode = KritaUtils::CopySnapshot, KisNode *newParentNode = 0);

protected:
    KisPaintDevice(KisDataManagerSP explicitDataManager,
                   KisPaintDeviceSP src, const QString& name = QString());

public:

    bool write(KisPaintDeviceWriter &store);

    bool read(QIODevice *stream);

public:

    void setParentNode(KisNodeWSP parent);

    void setDefaultBounds(KisDefaultBoundsBaseSP bounds);

    KisDefaultBoundsBaseSP defaultBounds() const;

    void moveTo(qint32 x, qint32 y);

    virtual void moveTo(const QPoint& pt);

    QPoint offset() const;

    qint32 x() const;

    qint32 y() const;

    void setX(qint32 x);

    void setY(qint32 y);

    QRect extent() const;

    void extent(qint32 &x, qint32 &y, qint32 &w, qint32 &h) const;

    QRect exactBounds() const;

    QRect exactBoundsAmortized() const;

    QRect nonDefaultPixelArea() const;

    KisRegion region() const;

    KisRegion regionExact() const;

    void crop(qint32 x, qint32 y, qint32 w, qint32 h);

    void crop(const QRect & r);

    virtual void clear();

    void clear(const QRect & rc);

    void purgeDefaultPixels();

    void setDefaultPixel(const KoColor &defPixel);

    KoColor defaultPixel() const;

    void fill(const QRect & rc, const KoColor &color);

    void fill(qint32 x, qint32 y, qint32 w, qint32 h, const quint8 *fillPixel);

public:

    void prepareClone(KisPaintDeviceSP src);

    void makeCloneFrom(KisPaintDeviceSP src, const QRect &rect);

    void makeCloneFromRough(KisPaintDeviceSP src, const QRect &minimalRect);

protected:
    friend class KisPaintDeviceTest;
    friend class DataReaderThread;

    bool fastBitBltPossible(KisPaintDeviceSP src);

    void fastBitBlt(KisPaintDeviceSP src, const QRect &rect);

    void fastBitBltOldData(KisPaintDeviceSP src, const QRect &rect);

    void fastBitBltRough(KisPaintDeviceSP src, const QRect &rect);

    void fastBitBltRoughOldData(KisPaintDeviceSP src, const QRect &rect);

public:
    void readBytes(quint8 * data, qint32 x, qint32 y, qint32 w, qint32 h) const;

    void readBytes(quint8 * data, const QRect &rect) const;

    void writeBytes(const quint8 * data, qint32 x, qint32 y, qint32 w, qint32 h);

    void writeBytes(const quint8 * data, const QRect &rect);

    QVector<quint8*> readPlanarBytes(qint32 x, qint32 y, qint32 w, qint32 h) const;

    void writePlanarBytes(QVector<quint8*> planes, qint32 x, qint32 y, qint32 w, qint32 h);

    void convertTo(const KoColorSpace *dstColorSpace,
                   KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                   KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags(),
                   KUndo2Command *parentCommand = nullptr,
                   KoUpdater *progressUpdater = nullptr);

    bool setProfile(const KoColorProfile * profile, KUndo2Command *parentCommand);

    void convertFromQImage(const QImage& image, const KoColorProfile *profile, qint32 offsetX = 0, qint32 offsetY = 0);

    QImage convertToQImage(const KoColorProfile *dstProfile, qint32 x, qint32 y, qint32 w, qint32 h,
                           KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                           KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags()) const;

    QImage convertToQImage(const KoColorProfile *dstProfile,
                           const QRect &rc,
                           KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                           KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags()) const;

    QImage convertToQImage(const KoColorProfile *  dstProfile,
                           KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                           KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags()) const;

    KisPaintDeviceSP createThumbnailDevice(qint32 w, qint32 h, QRect rect = QRect(), QRect outputRect = QRect()) const;
    KisPaintDeviceSP createThumbnailDeviceOversampled(qint32 w, qint32 h, qreal oversample, QRect rect = QRect(),  QRect outputRect = QRect()) const;

    QImage createThumbnailUncached(qint32 maxw, qint32 maxh, QRect rect, qreal oversample = 1,
                           KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                           KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags());

    QImage createThumbnail(qint32 maxw, qint32 maxh,
                           KisThumbnailBoundsMode boundsMode = KisThumbnailBoundsMode::Precise,
                           qreal oversample = 1,
                           KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                           KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags());

    QImage createThumbnail(qint32 maxw, qint32 maxh,
                           Qt::AspectRatioMode aspectRatioMode,
                           KisThumbnailBoundsMode boundsMode = KisThumbnailBoundsMode::Precise,
                           qreal oversample = 1,
                           KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                           KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags());

    bool pixel(qint32 x, qint32 y, QColor *c) const;

    bool pixel(qint32 x, qint32 y, KoColor * kc) const;

    KoColor pixel(const QPoint &pos) const;

    bool setPixel(qint32 x, qint32 y, const QColor& c);

    bool setPixel(qint32 x, qint32 y, const KoColor& kc);

    const KoColorSpace* colorSpace() const;

    KisPaintDeviceSP createCompositionSourceDevice() const;

    KisPaintDeviceSP createCompositionSourceDevice(KisPaintDeviceSP cloneSource) const;

    KisPaintDeviceSP createCompositionSourceDevice(KisPaintDeviceSP cloneSource, const QRect roughRect) const;

    KisFixedPaintDeviceSP createCompositionSourceDeviceFixed() const;

    virtual const KoColorSpace* compositionSourceColorSpace() const;

    KisDataManagerSP dataManager() const;

    void setDataManager(KisDataManagerSP data, const KoColorSpace * colorSpace = 0);

    quint32 pixelSize() const;

    quint32 channelCount() const;

    KisInterstrokeDataSP interstrokeData() const;

    KUndo2Command* createChangeInterstrokeDataCommand(KisInterstrokeDataSP data);

    KisRasterKeyframeChannel *createKeyframeChannel(const KoID &id);

    KisRasterKeyframeChannel* keyframeChannel() const;

    KisPaintDeviceFramesInterface* framesInterface();

    bool burnKeyframe(int frameID);
    bool burnKeyframe();

public:

    void setDirty(const QRect & rc);

    void setDirty(const KisRegion &region);

    void setDirty();

    void setDirty(const QVector<QRect> &rects);

    void requestTimeSwitch(int time);

    int sequenceNumber() const;

    void estimateMemoryStats(qint64 &imageData, qint64 &temporaryData, qint64 &lodData) const;

public:

    KisHLineIteratorSP createHLineIteratorNG(qint32 x, qint32 y, qint32 w);
    KisHLineConstIteratorSP createHLineConstIteratorNG(qint32 x, qint32 y, qint32 w) const;

    KisVLineIteratorSP createVLineIteratorNG(qint32 x, qint32 y, qint32 h);
    KisVLineConstIteratorSP createVLineConstIteratorNG(qint32 x, qint32 y, qint32 h) const;

    KisRandomAccessorSP createRandomAccessorNG();
    KisRandomConstAccessorSP createRandomConstAccessorNG() const;

    KisRepeatHLineConstIteratorSP createRepeatHLineConstIterator(qint32 x, qint32 y, qint32 w, const QRect& _dataWidth) const;
    KisRepeatVLineConstIteratorSP createRepeatVLineConstIterator(qint32 x, qint32 y, qint32 h, const QRect& _dataWidth) const;

    KisRandomSubAccessorSP createRandomSubAccessor() const;

    void clearSelection(KisSelectionSP selection);

    KUndo2Command* reincarnateWithDetachedHistory(bool copyContent);

Q_SIGNALS:

    void profileChanged(const KoColorProfile *  profile);
    void colorSpaceChanged(const KoColorSpace *colorspace);

public:
    friend class PaintDeviceCache;

    QRect calculateExactBounds(bool nonDefaultOnly) const;

public:
    struct MemoryReleaseObject : public QObject {
        ~MemoryReleaseObject() override;
    };

    static MemoryReleaseObject* createMemoryReleaseObject();

public:
    struct LodDataStruct {
        virtual ~LodDataStruct();
    };

    KisRegion regionForLodSyncing() const;
    LodDataStruct* createLodDataStruct(int lod);
    void updateLodDataStruct(LodDataStruct *dst, const QRect &srcRect);
    void uploadLodDataStruct(LodDataStruct *dst);

    void generateLodCloneDevice(KisPaintDeviceSP dst, const QRect &originalRect, int lod);

    void setSupportsWraparoundMode(bool value);
    bool supportsWraproundMode() const;

    void setProjectionDevice(bool value);
    void testingFetchLodDevice(KisPaintDeviceSP targetDevice);

private:
    KisPaintDevice& operator=(const KisPaintDevice&);
    void init(const KoColorSpace *colorSpace,
              KisDefaultBoundsBaseSP defaultBounds,
              KisNodeWSP parent, const QString& name);

    friend class KisPainter;

    QVector<qint32> channelSizes() const;

    void emitColorSpaceChanged();
    void emitProfileChanged();

private:
    friend class KisPaintDeviceFramesInterface;

protected:
    friend class KisSelectionTest;
    KisNodeWSP parentNode() const;

private:
    struct Private;
    Private * const m_d;
};

#endif
