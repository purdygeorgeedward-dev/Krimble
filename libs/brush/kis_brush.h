/*
 *  SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_BRUSH_
#define KIS_BRUSH_

#include <QImage>

#include <KoResource.h>

#include <kis_types.h>
#include <kis_shared.h>
#include <kis_dab_shape.h>
#include <kritabrush_export.h>

class QString;
class KoColor;
class KoColorSpace;

class KisPaintInformation;
class KisPaintopLodLimitations;
class KoAbstractGradient;
class KisOptimizedBrushOutline;
typedef QSharedPointer<KoAbstractGradient> KoAbstractGradientSP;

enum enumBrushType {
    INVALID,
    MASK,
    IMAGE,
    PIPE_MASK,
    PIPE_IMAGE
};

enum enumBrushApplication {
    ALPHAMASK,
    IMAGESTAMP,
    LIGHTNESSMAP,
    GRADIENTMAP
};

static const qreal DEFAULT_SOFTNESS_FACTOR = 1.0;
static const qreal DEFAULT_LIGHTNESS_STRENGTH = 1.0;

class KisBrush;
typedef QSharedPointer<KisBrush> KisBrushSP;

class BRUSH_EXPORT KisBrush : public KoResource
{
public:
    class ColoringInformation
    {
    public:
        virtual ~ColoringInformation();
        virtual const quint8* color() const = 0;
        virtual void nextColumn() = 0;
        virtual void nextRow() = 0;
    };

protected:

    class PlainColoringInformation : public ColoringInformation
    {
    public:
        PlainColoringInformation(const quint8* color);
        ~PlainColoringInformation() override;
        const quint8* color() const override ;
        void nextColumn() override;
        void nextRow() override;
    private:
        const quint8* m_color;
    };

    class PaintDeviceColoringInformation : public ColoringInformation
    {

    public:

        PaintDeviceColoringInformation(const KisPaintDeviceSP source, int width);
        ~PaintDeviceColoringInformation() override;
        const quint8* color() const override ;
        void nextColumn() override;
        void nextRow() override;

    private:

        const KisPaintDeviceSP m_source;
        KisHLineConstIteratorSP m_iterator;
    };

public:

    KisBrush();
    KisBrush(const QString& filename);
    ~KisBrush() override;

    KisBrush(const KisBrush &rhs);
    KisBrush &operator=(const KisBrush &rhs) = delete;

    virtual qreal userEffectiveSize() const = 0;
    virtual void setUserEffectiveSize(qreal value) = 0;

    QPair<QString, QString> resourceType() const override {
        return QPair<QString, QString>(ResourceType::Brushes, "");
    }

    virtual QImage brushTipImage() const;

    virtual KisFixedPaintDeviceSP outlineSourceImage() const;

    virtual void setSpacing(double spacing);

    double spacing() const;

    void setAutoSpacing(bool active, qreal coeff);

    bool autoSpacingActive() const;
    qreal autoSpacingCoeff() const;

    qint32 width() const;

    qint32 height() const;

    virtual qint32 maskWidth(KisDabShape const&, qreal subPixelX, qreal subPixelY, const KisPaintInformation& info) const;

    virtual qint32 maskHeight(KisDabShape const&, qreal subPixelX, qreal subPixelY, const KisPaintInformation& info) const;

     virtual QSizeF characteristicSize(KisDabShape const&) const;

    double maskAngle(double angle = 0) const;

    virtual quint32 brushIndex() const;

    virtual enumBrushType brushType() const;

    QPointF hotSpot(KisDabShape const&, const KisPaintInformation& info) const;

    virtual bool canPaintFor(const KisPaintInformation& );

    virtual void notifyStrokeStarted();

    virtual void notifyBrushIsGoingToBeClonedForStroke();

    virtual void prepareForSeqNo(const KisPaintInformation& info, int seqNo);

    virtual KisFixedPaintDeviceSP paintDevice(const KoColorSpace * colorSpace,
            KisDabShape const&,
            const KisPaintInformation& info,
            double subPixelX = 0, double subPixelY = 0) const;

    void mask(KisFixedPaintDeviceSP dst,
              const KoColor& color,
              KisDabShape const& shape,
              const KisPaintInformation& info,
              double subPixelX = 0, double subPixelY = 0,
              qreal softnessFactor = DEFAULT_SOFTNESS_FACTOR, qreal lightnessStrength = DEFAULT_LIGHTNESS_STRENGTH) const;

    void mask(KisFixedPaintDeviceSP dst,
              const KisPaintDeviceSP src,
              KisDabShape const& shape,
              const KisPaintInformation& info,
              double subPixelX = 0, double subPixelY = 0,
              qreal softnessFactor = DEFAULT_SOFTNESS_FACTOR, qreal lightnessStrength = DEFAULT_LIGHTNESS_STRENGTH) const;

    virtual enumBrushApplication brushApplication() const;

    virtual void setBrushApplication(enumBrushApplication brushApplication);

    virtual bool preserveLightness() const;

    virtual bool applyingGradient() const;

    virtual void setGradient(KoAbstractGradientSP gradient);

    virtual void generateMaskAndApplyMaskOrCreateDab(KisFixedPaintDeviceSP dst,
            ColoringInformation* coloringInfo,
            KisDabShape const&,
            const KisPaintInformation& info,
            double subPixelX, double subPixelY,
            qreal softnessFactor, qreal lightnessStrength) const;

    void generateMaskAndApplyMaskOrCreateDab(KisFixedPaintDeviceSP dst,
        ColoringInformation* coloringInfo,
        KisDabShape const&,
        const KisPaintInformation& info,
        double subPixelX = 0, double subPixelY = 0,
        qreal softnessFactor = DEFAULT_SOFTNESS_FACTOR) const;

    virtual void toXML(QDomDocument& , QDomElement&) const;

    static KisBrushSP fromXML(const QDomElement& element, KisResourcesInterfaceSP resourcesInterface);

    static KoResourceLoadResult fromXMLLoadResult(const QDomElement& element, KisResourcesInterfaceSP resourcesInterface);

    virtual KisOptimizedBrushOutline outline(bool forcePreciseOutline = false) const;

    virtual void setScale(qreal _scale);
    qreal scale() const;
    virtual void setAngle(qreal _angle);
    qreal angle() const;

    void clearBrushPyramid();

    virtual void lodLimitations(KisPaintopLodLimitations *l) const;

    virtual bool supportsCaching() const;

    virtual void coldInitBrush();

    static const QString brushTypeMetaDataKey;

protected:

    void setWidth(qint32 width);

    void setHeight(qint32 height);

    void setHotSpot(QPointF);

    virtual void setBrushType(enumBrushType type);

public:

    virtual void setBrushTipImage(const QImage& image);

    virtual bool isPiercedApprox() const;

protected:

    void resetOutlineCache();
    void generateOutlineCache();
    bool outlineCacheIsValid() const;

    void predefinedBrushToXML(const QString &type, QDomElement& e) const;

private:

    struct Private;
    Private* const d;

};

Q_DECLARE_METATYPE(KisBrushSP)

#endif

