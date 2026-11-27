/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_IMAGEPIPE_BRUSH_
#define KIS_IMAGEPIPE_BRUSH_

#include <QMap>
#include <QString>

#include <KoResource.h>
#include "kis_gbr_brush.h"
#include "kis_global.h"

class KisPipeBrushParasite;

namespace KisParasite
{
enum SelectionMode {
    Constant,
    Incremental,
    Angular,
    Velocity,
    Random,
    Pressure,
    TiltX,
    TiltY
};
}

class BRUSH_EXPORT KisImagePipeBrush : public KisGbrBrush
{

public:
    KisImagePipeBrush(const QString& filename);
    KisImagePipeBrush(const QString& name, int w, int h,
                      QVector< QVector<KisPaintDevice*> > devices,
                      QVector<KisParasite::SelectionMode> modes);

    ~KisImagePipeBrush() override;

    KisImagePipeBrush(const KisImagePipeBrush& rhs);

    KisImagePipeBrush &operator=(const KisImagePipeBrush &rhs) = delete;

    KoResourceSP clone() const override;

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice* dev) const override;

    KisFixedPaintDeviceSP paintDevice(const KoColorSpace * colorSpace,
            KisDabShape const&,
            const KisPaintInformation& info,
            double subPixelX = 0, double subPixelY = 0) const override;

    void setAdjustmentMidPoint(quint8 value) override;
    void setBrightnessAdjustment(qreal value) override;
    void setContrastAdjustment(qreal value) override;
    void setAutoAdjustMidPoint(bool value) override;

    QString parasiteSelection();

    KisOptimizedBrushOutline outline(bool forcePreciseOutline = false) const override;

    bool canPaintFor(const KisPaintInformation& info) override;

    void makeMaskImage(bool preserveAlpha) override;

    QString defaultFileExtension() const override;
    void setAngle(qreal _angle) override;
    void setScale(qreal _scale) override;
    void setSpacing(double _spacing) override;

    quint32 brushIndex() const override;
    qint32 maskWidth(KisDabShape const&, double subPixelX, double subPixelY, const KisPaintInformation& info) const override;
    qint32 maskHeight(KisDabShape const&, double subPixelX, double subPixelY, const KisPaintInformation& info) const override;

    void notifyStrokeStarted() override;
    void prepareForSeqNo(const KisPaintInformation& info, int seqNo) override;

    void generateMaskAndApplyMaskOrCreateDab(KisFixedPaintDeviceSP dst, KisBrush::ColoringInformation* coloringInformation,
            KisDabShape const&,
            const KisPaintInformation& info,
            double subPixelX = 0, double subPixelY = 0,
            qreal softnessFactor = DEFAULT_SOFTNESS_FACTOR, qreal lightnessStrength = DEFAULT_LIGHTNESS_STRENGTH) const override;

    void notifyBrushIsGoingToBeClonedForStroke() override;

    QVector<KisGbrBrushSP> brushes() const;

    const KisPipeBrushParasite &parasite() const;

    void setParasite(const KisPipeBrushParasite& parasite);
    void setDevices(QVector< QVector<KisPaintDevice*> > devices, int w, int h);

    void coldInitBrush() override;

protected:
    virtual void setBrushApplication(enumBrushApplication brushApplication) override;
    virtual void setGradient(KoAbstractGradientSP gradient) override;

private:
    friend class KisImagePipeBrushTest;

    KisGbrBrushSP testingGetCurrentBrush(const KisPaintInformation& info) const;
    void testingSelectNextBrush(const KisPaintInformation& info) const;

    bool initFromData(const QByteArray &data);

    QString parasiteSelectionString;

private:
    struct Private;
    Private * const d;
};

typedef QSharedPointer<KisImagePipeBrush> KisImagePipeBrushSP;

#endif
