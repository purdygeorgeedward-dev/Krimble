/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_DISPLAY_COLOR_CONVERTER_H
#define __KIS_DISPLAY_COLOR_CONVERTER_H

#include <QScopedPointer>

#include <KoColorDisplayRendererInterface.h>
#include <KoColorConversionTransformation.h>

#include "KisHandleStyle.h"
#include "kis_types.h"
#include "canvas/kis_display_filter.h"

class KoColor;
class KoColorProfile;
class KoCanvasResourceProvider;
class KisDisplayConfig;
class KisMultiSurfaceDisplayConfig;
class KoID;
class QPalette;

class KRITAUI_EXPORT KisDisplayColorConverter : public QObject
{
    Q_OBJECT

public:
    KisDisplayColorConverter();
    KisDisplayColorConverter(KoCanvasResourceProvider *resourceManager, QObject *parent);
    ~KisDisplayColorConverter() override;

    void setImage(KisImageSP image);
    void setImageColorSpace(const KoColorSpace *cs);

    static KisDisplayColorConverter* dumbConverterInstance();

    KoColorDisplayRendererInterface* displayRendererInterface() const;

    const KoColorSpace* paintingColorSpace() const;
    const KoColorSpace* nodeColorSpace() const;
    void setMultiSurfaceDisplayConfig(const KisMultiSurfaceDisplayConfig &config);
    void setDisplayFilter(QSharedPointer<KisDisplayFilter> displayFilter);

    QColor toQColor(const KoColor &c, bool proofToPaintColors = false) const;
    KoColor approximateFromRenderedQColor(const QColor &c) const;

    bool canSkipDisplayConversion(const KoColorSpace *cs) const;
    KoColor applyDisplayFiltering(const KoColor &srcColor, const KoID &bitDepthId) const;

    void applyDisplayFilteringF32(KisFixedPaintDeviceSP device, const KoColorSpace *dstColorSpace) const;

    QColor convertColorToDisplayColorSpace(const KoColor color, bool applyOcio = false) const;

    QImage convertImageToDisplayColorSpace(KisPaintDeviceSP srcDevice, QRect source = QRect(), bool applyOcio = false) const;

    KisHandlePalette handlePaletteForDisplayColorSpace() const;

    QPalette systemPaletteForDisplayColorSpace() const;

    QImage toQImage(KisPaintDeviceSP srcDevice, bool proofPaintColors = false) const;
    QImage toQImage(const KoColorSpace *srcColorSpace, const quint8 *data, QSize size, bool proofPaintColors = false) const;

    KoColor fromHsv(int h, int s, int v, int a = 255) const;
    KoColor fromHsvF(qreal h, qreal s, qreal v, qreal a = 1.0);
    KoColor fromHslF(qreal h, qreal s, qreal l, qreal a = 1.0);
    KoColor fromHsiF(qreal h, qreal s, qreal i);
    KoColor fromHsyF(qreal h, qreal s, qreal y, qreal R=0.2126, qreal G=0.7152, qreal B=0.0722, qreal gamma=2.2);

    void getHsv(const KoColor &srcColor, int *h, int *s, int *v, int *a = 0) const;
    void getHsvF(const KoColor &srcColor, qreal *h, qreal *s, qreal *v, qreal *a = 0);
    void getHslF(const KoColor &srcColor, qreal *h, qreal *s, qreal *l, qreal *a = 0);
    void getHsiF(const KoColor &srcColor, qreal *h, qreal *s, qreal *i);
    void getHsyF(const KoColor &srcColor, qreal *h, qreal *s, qreal *y, qreal R=0.2126, qreal G=0.7152, qreal B=0.0722, qreal gamma=2.2);

    KisDisplayConfig displayConfig() const;

    QSharedPointer<KisDisplayFilter> displayFilter() const;
    KisMultiSurfaceDisplayConfig multiSurfaceDisplayConfig() const;

    using ConversionOptions = std::pair<KoColorConversionTransformation::Intent, KoColorConversionTransformation::ConversionFlags>;
    ConversionOptions conversionOptions() const;

Q_SIGNALS:
    void displayConfigurationChanged();

private:
    KoColor toKoColor(const QColor &c);
    template <class Policy>
    typename Policy::Result convertToDisplayImpl(const KoColor &srcColor, bool alreadyInDestinationF32 = false) const;

private:
    Q_PRIVATE_SLOT(m_d, void slotCanvasResourceChanged(int key, const QVariant &v));
    Q_PRIVATE_SLOT(m_d, void selectPaintingColorSpace());
    Q_PRIVATE_SLOT(m_d, void slotUpdateCurrentNodeColorSpace());
    Q_SLOT void updatePalettes();

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
