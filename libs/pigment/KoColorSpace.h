/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOCOLORSPACE_H
#define KOCOLORSPACE_H

#include <climits>

#include <QImage>
#include <QHash>
#include <QVector>
#include <QList>

#include <boost/operators.hpp>
#include "KoColorSpaceConstants.h"
#include "KoColorConversionTransformation.h"
#include "KoColorProofingConversionTransformation.h"
#include "KoCompositeOp.h"
#include "KisDitherOp.h"
#include <KoID.h>
#include "kritapigment_export.h"

class QDomDocument;
class QDomElement;
class KoChannelInfo;
class KoColorProfile;
class KoColorTransformation;
class QBitArray;

enum Deletability {
    OwnedByRegistryDoNotDelete,
    OwnedByRegistryRegistryDeletes,
    NotOwnedByRegistry
};

enum ColorSpaceIndependence {
    FULLY_INDEPENDENT,
    TO_LAB16,
    TO_RGBA8,
    TO_RGBA16
};

class KoMixColorsOp;
class KoConvolutionOp;

class KRITAPIGMENT_EXPORT KoColorSpace : public boost::equality_comparable<KoColorSpace>
{
    friend class KoColorSpaceRegistry;
    friend class KoColorSpaceFactory;
protected:
    KoColorSpace();

public:

    KoColorSpace(const QString &id, const QString &name, KoMixColorsOp *mixColorsOp, KoConvolutionOp *convolutionOp);

    virtual bool operator==(const KoColorSpace& rhs) const;
protected:
    virtual ~KoColorSpace();

public:
    QPolygonF gamutXYY() const;

    QPolygonF estimatedTRCXYY() const;

    QVector <qreal> lumaCoefficients() const;

    QList<KoChannelInfo *> channels() const;

    virtual quint32 channelCount() const = 0;

    virtual quint32 alphaPos() const = 0;

    virtual quint32 colorChannelCount() const = 0;

    QBitArray channelFlags(bool color = true, bool alpha = false) const;

    virtual quint32 pixelSize() const = 0;

    virtual QString channelValueText(const quint8 *pixel, quint32 channelIndex) const = 0;

    virtual QString normalisedChannelValueText(const quint8 *pixel, quint32 channelIndex) const = 0;

    virtual void normalisedChannelsValue(const quint8 *pixel, QVector<float> &channels) const = 0;

    virtual void fromNormalisedChannelsValue(quint8 *pixel, const QVector<float> &values) const = 0;

    virtual quint8 scaleToU8(const quint8 * srcPixel, qint32 channelPos) const = 0;

    virtual void singleChannelPixel(quint8 *dstPixel, const quint8 *srcPixel, quint32 channelIndex) const = 0;

    QString id() const;

    QString name() const;

    virtual KoID colorModelId() const = 0;
    virtual KoID colorDepthId() const = 0;

    virtual bool profileIsCompatible(const KoColorProfile* profile) const = 0;

    virtual bool willDegrade(ColorSpaceIndependence independence) const = 0;

    virtual bool hasCompositeOp(const QString & id, const KoColorSpace *srcSpace = nullptr) const;

    virtual QList<KoCompositeOp*> compositeOps() const;

    const KoCompositeOp * compositeOp(const QString & id, const KoColorSpace *srcSpace = nullptr) const;

    virtual void addCompositeOp(const KoCompositeOp * op);

    virtual bool hasHighDynamicRange() const = 0;

    virtual const KoColorProfile * profile() const = 0;

    virtual void transparentColor(quint8 *dst, quint32 nPixels) const;

    virtual void fromQColor(const QColor& color, quint8 *dst) const = 0;

    virtual void toQColor(const quint8 *src, QColor *c) const = 0;

    virtual void toQColor16(const quint8 *src, QColor *c) const = 0;

    virtual QImage convertToQImage(const quint8 *data, qint32 width, qint32 height,
                                   const KoColorProfile *  dstProfile,
                                   KoColorConversionTransformation::Intent renderingIntent,
                                   KoColorConversionTransformation::ConversionFlags conversionFlags) const;

    virtual void toLabA16(const quint8 * src, quint8 * dst, quint32 nPixels) const;

    virtual void fromLabA16(const quint8 * src, quint8 * dst, quint32 nPixels) const;

    virtual void toRgbA16(const quint8 * src, quint8 * dst, quint32 nPixels) const;

    virtual void fromRgbA16(const quint8 * src, quint8 * dst, quint32 nPixels) const;

    virtual KoColorConversionTransformation* createColorConverter(const KoColorSpace * dstColorSpace,
                                                                  KoColorConversionTransformation::Intent renderingIntent,
                                                                  KoColorConversionTransformation::ConversionFlags conversionFlags) const;

    virtual const KisDitherOp *ditherOp(const QString &depth, DitherType type) const;

    virtual void addDitherOp(KisDitherOp *op);

    virtual bool convertPixelsTo(const quint8 * src,
                                 quint8 * dst, const KoColorSpace * dstColorSpace,
                                 quint32 numPixels,
                                 KoColorConversionTransformation::Intent renderingIntent,
                                 KoColorConversionTransformation::ConversionFlags conversionFlags) const;

    virtual KoColorConversionTransformation *createProofingTransform(const KoColorSpace * dstColorSpace,
                                                             const KoColorSpace * proofingSpace,
                                                             KoColorConversionTransformation::Intent renderingIntent,
                                                             KoColorConversionTransformation::Intent proofingIntent,
                                                             bool bpcFirstTransform,
                                                             quint8 *gamutWarning,
                                                             KoColorConversionTransformation::ConversionFlags displayConversionFlags) const;
    virtual bool proofPixelsTo(const quint8 * src,
                               quint8 * dst,
                               quint32 numPixels,
                               KoColorConversionTransformation *proofingTransform) const;

    virtual void convertChannelToVisualRepresentation(const quint8 *src, quint8 *dst, quint32 nPixels, const qint32 selectedChannelIndex) const = 0;

    virtual void convertChannelToVisualRepresentation(const quint8 *src, quint8 *dst, quint32 nPixels, const QBitArray selectedChannels) const = 0;

    virtual quint8 opacityU8(const quint8 * pixel) const = 0;
    virtual qreal opacityF(const quint8 * pixel) const = 0;

    virtual void setOpacity(quint8 * pixels, quint8 alpha, qint32 nPixels) const = 0;
    virtual void setOpacity(quint8 * pixels, qreal alpha, qint32 nPixels) const = 0;

    virtual void copyOpacityU8(quint8* src, quint8* dst, qint32 nPixels) const = 0;

    virtual void multiplyAlpha(quint8 * pixels, quint8 alpha, qint32 nPixels) const = 0;

    virtual void applyAlphaU8Mask(quint8 * pixels, const quint8 * alpha, qint32 nPixels) const = 0;

    virtual void applyInverseAlphaU8Mask(quint8 * pixels, const quint8 * alpha, qint32 nPixels) const = 0;

    virtual void applyAlphaNormedFloatMask(quint8 * pixels, const float * alpha, qint32 nPixels) const = 0;

    virtual void applyInverseNormedFloatMask(quint8 * pixels, const float * alpha, qint32 nPixels) const = 0;

    virtual void fillInverseAlphaNormedFloatMaskWithColor(quint8 * pixels, const float * alpha, const quint8 *brushColor, qint32 nPixels) const = 0;

    virtual void fillGrayBrushWithColor(quint8 *dst, const QRgb *brush, quint8 *brushColor, qint32 nPixels) const = 0;

    virtual void fillGrayBrushWithColorAndLightnessOverlay(quint8 *dst, const QRgb *brush, quint8 *brushColor, qint32 nPixels) const;
    virtual void fillGrayBrushWithColorAndLightnessWithStrength(quint8* dst, const QRgb* brush, quint8* brushColor, qreal strength, qint32 nPixels) const;
    virtual void modulateLightnessByGrayBrush(quint8* dst, const QRgb *brush, qreal strength, qint32 nPixels) const;

    virtual KoColorTransformation *createBrightnessContrastAdjustment(const quint16 *transferValues) const = 0;

    virtual KoColorTransformation *createPerChannelAdjustment(const quint16 * const* transferValues) const = 0;

    virtual KoColorTransformation *createDarkenAdjustment(qint32 shade, bool compensate, qreal compensation) const = 0;

    virtual KoColorTransformation *createInvertTransformation() const = 0;

    virtual quint8 difference(const quint8* src1, const quint8* src2) const = 0;

    virtual quint8 differenceA(const quint8* src1, const quint8* src2) const = 0;

    virtual KoMixColorsOp* mixColorsOp() const;

    virtual KoConvolutionOp* convolutionOp() const;

    virtual quint8 intensity8(const quint8 * src) const = 0;

    virtual qreal intensityF(const quint8 * src) const = 0;

    virtual void increaseLuminosity(quint8 * pixel, qreal step) const;
    virtual void decreaseLuminosity(quint8 * pixel, qreal step) const;
    virtual void increaseSaturation(quint8 * pixel, qreal step) const;
    virtual void decreaseSaturation(quint8 * pixel, qreal step) const;
    virtual void increaseHue(quint8 * pixel, qreal step) const;
    virtual void decreaseHue(quint8 * pixel, qreal step) const;
    virtual void increaseRed(quint8 * pixel, qreal step) const;
    virtual void increaseGreen(quint8 * pixel, qreal step) const;
    virtual void increaseBlue(quint8 * pixel, qreal step) const;
    virtual void increaseYellow(quint8 * pixel, qreal step) const;
    virtual void toHSY(const QVector<double> &channelValues, qreal *hue, qreal *sat, qreal *luma) const = 0;
    virtual QVector <double> fromHSY(qreal *hue, qreal *sat, qreal *luma) const = 0;
    virtual void toYUV(const QVector<double> &channelValues, qreal *y, qreal *u, qreal *v) const = 0;
    virtual QVector <double> fromYUV(qreal *y, qreal *u, qreal *v) const = 0;
    virtual void bitBlt(const KoColorSpace* srcSpace, const KoCompositeOp::ParameterInfo& params, const KoCompositeOp* op,
                        KoColorConversionTransformation::Intent renderingIntent,
                        KoColorConversionTransformation::ConversionFlags conversionFlags) const;

    virtual void colorToXML(const quint8* pixel, QDomDocument& doc, QDomElement& colorElt) const = 0;

    virtual void colorFromXML(quint8* pixel, const QDomElement& elt) const = 0;

    KoColorTransformation* createColorTransformation(const QString & id, const QHash<QString, QVariant> & parameters) const;

protected:

    virtual void addChannel(KoChannelInfo * ci);
    const KoColorConversionTransformation* toLabA16Converter() const;
    const KoColorConversionTransformation* fromLabA16Converter() const;
    const KoColorConversionTransformation* toRgbA16Converter() const;
    const KoColorConversionTransformation* fromRgbA16Converter() const;

    virtual bool preferCompositionInSourceColorSpace() const;

    struct Private;
    Private * const d;

};

inline QDebug operator<<(QDebug dbg, const KoColorSpace *cs)
{
    if (cs) {
        dbg.nospace() << cs->name() << " (" << cs->colorModelId().id() << "," << cs->colorDepthId().id() << " )";
    } else {
        dbg.nospace() << "0x0";
    }

    return dbg.space();
}

#endif
