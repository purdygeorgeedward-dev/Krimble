/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef __KO_COLOR_DISPLAY_RENDERER_INTERFACE_H
#define __KO_COLOR_DISPLAY_RENDERER_INTERFACE_H

#include <QObject>
#include <QColor>

#include "KoColor.h"

class KoChannelInfo;
class KoColorSpace;
struct KisHandlePalette;
class QPalette;

class KRITAPIGMENT_EXPORT KoColorDisplayRendererInterface : public QObject
{
    Q_OBJECT

public:
    KoColorDisplayRendererInterface();
    ~KoColorDisplayRendererInterface() override;

    virtual QImage toQImage(const KoColorSpace *srcColorSpace, const quint8 *data, QSize size, bool proofPaintColors = false) const = 0;

    virtual QColor toQColor(const KoColor &c, bool proofToPaintColors = false) const = 0;

    virtual KoColor approximateFromRenderedQColor(const QColor &c) const = 0;

    virtual KoColor fromHsv(int h, int s, int v, int a = 255) const = 0;
    virtual void getHsv(const KoColor &srcColor, int *h, int *s, int *v, int *a = 0) const = 0;

    virtual qreal minVisibleFloatValue(const KoChannelInfo *chaninfo) const = 0;

    virtual qreal maxVisibleFloatValue(const KoChannelInfo *chaninfo) const = 0;

    virtual const KoColorSpace* getPaintingColorSpace() const = 0;

    virtual QColor convertColorToDisplayColorSpace(const KoColor color) const = 0;

    virtual QImage convertImageToDisplayColorSpace(const QImage source) const = 0;

    virtual KisHandlePalette handlePaletteForDisplayColorSpace() const = 0;

    virtual QPalette systemPaletteForDisplayColorSpace() const = 0;

Q_SIGNALS:
    void displayConfigurationChanged();

private:
    Q_DISABLE_COPY(KoColorDisplayRendererInterface)
};

class KRITAPIGMENT_EXPORT KoDumbColorDisplayRenderer : public KoColorDisplayRendererInterface
{
public:
    QImage toQImage(const KoColorSpace *srcColorSpace, const quint8 *data, QSize size, bool proofPaintColors = false) const override;
    QColor toQColor(const KoColor &c, bool proofToPaintColors = false) const override;
    KoColor approximateFromRenderedQColor(const QColor &c) const override;
    KoColor fromHsv(int h, int s, int v, int a = 255) const override;
    void getHsv(const KoColor &srcColor, int *h, int *s, int *v, int *a = 0) const override;

    qreal minVisibleFloatValue(const KoChannelInfo *chaninfo) const override;
    qreal maxVisibleFloatValue(const KoChannelInfo *chaninfo) const override;

    const KoColorSpace* getPaintingColorSpace() const override;

    QColor convertColorToDisplayColorSpace(KoColor c) const override;
    QImage convertImageToDisplayColorSpace(const QImage source) const override;
    KisHandlePalette handlePaletteForDisplayColorSpace() const override;
    QPalette systemPaletteForDisplayColorSpace() const override;

    static KoColorDisplayRendererInterface* instance();
};

#endif
