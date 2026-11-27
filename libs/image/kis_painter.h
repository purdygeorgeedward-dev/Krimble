/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004 Clarence Dang <dang@kde.org>
 *  SPDX-FileCopyrightText: 2008-2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *  SPDX-FileCopyrightText: 2010 José Luis Vergara Toloza <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PAINTER_H_
#define KIS_PAINTER_H_

#include <math.h>

#include <QVector>

#include <KoColorSpaceConstants.h>
#include <KoColorConversionTransformation.h>
#include <KoPattern.h>
#include <KoAbstractGradient.h>

#include "kundo2magicstring.h"
#include "kis_types.h"
#include <kis_filter_configuration.h>
#include <kritaimage_export.h>

class QPen;
class KUndo2Command;
class QRect;
class QRectF;
class QBitArray;
class QPainterPath;

class KoUpdater;
class KoColor;
class KoCompositeOp;

class KisUndoAdapter;
class KisPostExecutionUndoAdapter;
class KisTransaction;
class KisPaintInformation;
class KisPaintOp;
class KisDistanceInformation;
struct KisRenderedDab;
class KisRunnableStrokeJobsInterface;

class KRITAIMAGE_EXPORT KisPainter
{

public:
    KisPainter();

    KisPainter(KisPaintDeviceSP device);

    KisPainter(KisPaintDeviceSP device, KisSelectionSP selection);
    virtual ~KisPainter();

public:
    static void copyAreaOptimized(const QPoint &dstPt,
                                  KisPaintDeviceSP src,
                                  KisPaintDeviceSP dst,
                                  const QRect &originalSrcRect);

    static void copyAreaOptimizedOldData(const QPoint &dstPt,
                                         KisPaintDeviceSP src,
                                         KisPaintDeviceSP dst,
                                         const QRect &originalSrcRect);

    static void copyAreaOptimized(const QPoint &dstPt,
                                  KisPaintDeviceSP src,
                                  KisPaintDeviceSP dst,
                                  const QRect &originalSrcRect,
                                  KisSelectionSP selection);

    static KisPaintDeviceSP convertToAlphaAsAlpha(KisPaintDeviceSP src);
    static KisPaintDeviceSP convertToAlphaAsGray(KisPaintDeviceSP src);

    static KisPaintDeviceSP convertToAlphaAsPureAlpha(KisPaintDeviceSP src);

    static bool checkDeviceHasTransparency(KisPaintDeviceSP dev);

    void begin(KisPaintDeviceSP device);

    void begin(KisPaintDeviceSP device, KisSelectionSP selection);

    void end();

    void setProgress(KoUpdater * progressUpdater);

    void beginTransaction(const KUndo2MagicString& transactionName = KUndo2MagicString(),int timedID = -1);

    void revertTransaction();

    void endTransaction(KisUndoAdapter *undoAdapter);

    void endTransaction(KisPostExecutionUndoAdapter *undoAdapter);

    KUndo2Command* endAndTakeTransaction();

    void deleteTransaction();

    void putTransaction(KisTransaction* transaction);

    KisTransaction* takeTransaction();

    const KisPaintDeviceSP device() const;
    KisPaintDeviceSP device();

    void bitBlt(qint32 dstX, qint32 dstY,
                const KisPaintDeviceSP srcDev,
                qint32 srcX, qint32 srcY,
                qint32 srcWidth, qint32 srcHeight);

    void bitBlt(const QPoint & pos, const KisPaintDeviceSP srcDev, const QRect & srcRect);

    void bitBltOldData(qint32 dstX, qint32 dstY,
                       const KisPaintDeviceSP srcDev,
                       qint32 srcX, qint32 srcY,
                       qint32 srcWidth, qint32 srcHeight);

    void bitBltOldData(const QPoint & pos, const KisPaintDeviceSP srcDev, const QRect & srcRect);

    void bitBltWithFixedSelection(qint32 dstX, qint32 dstY,
                                  const KisPaintDeviceSP srcDev,
                                  const KisFixedPaintDeviceSP selection,
                                  qint32 selX, qint32 selY,
                                  qint32 srcX, qint32 srcY,
                                  qint32 srcWidth, qint32 srcHeight);

    void bitBltWithFixedSelection(qint32 dstX, qint32 dstY,
                                  const KisPaintDeviceSP srcDev,
                                  const KisFixedPaintDeviceSP selection,
                                  qint32 srcWidth, qint32 srcHeight);

    void bltFixed(qint32 dstX, qint32 dstY,
                  const KisFixedPaintDeviceSP srcDev,
                  qint32 srcX, qint32 srcY,
                  qint32 srcWidth, qint32 srcHeight);

    void bltFixed(const QRect &rc, const QList<KisRenderedDab> allSrcDevices);

    void bltFixed(const QPoint & pos, const KisFixedPaintDeviceSP srcDev, const QRect & srcRect);

    void bltFixedWithFixedSelection(qint32 dstX, qint32 dstY,
                                    const KisFixedPaintDeviceSP srcDev,
                                    const KisFixedPaintDeviceSP selection,
                                    qint32 selX, qint32 selY,
                                    qint32 srcX, qint32 srcY,
                                    quint32 srcWidth, quint32 srcHeight);

    void bltFixedWithFixedSelection(qint32 dstX, qint32 dstY,
                                    const KisFixedPaintDeviceSP srcDev,
                                    const KisFixedPaintDeviceSP selection,
                                    quint32 srcWidth, quint32 srcHeight);

    void fill(qint32 x, qint32 y, qint32 width, qint32 height, const KoColor& color);

    void renderMirrorMask(QRect rc, KisFixedPaintDeviceSP dab);
    void renderMirrorMask(QRect rc, KisFixedPaintDeviceSP dab, KisFixedPaintDeviceSP mask);
    void renderMirrorMask(QRect rc, KisPaintDeviceSP dab);
    void renderMirrorMask(QRect rc, KisPaintDeviceSP dab, int sx, int sy, KisFixedPaintDeviceSP mask);

    void renderMirrorMaskSafe(QRect rc, KisFixedPaintDeviceSP dab, bool preserveDab);
    void renderMirrorMaskSafe(QRect rc, KisFixedPaintDeviceSP dab, KisFixedPaintDeviceSP mask, bool preserveDab);

    void renderMirrorMaskSafe(QRect rc, KisPaintDeviceSP dab, int sx, int sy, KisFixedPaintDeviceSP mask, bool preserveMask);

    void renderDabWithMirroringNonIncremental(QRect rc, KisPaintDeviceSP dab);

    bool hasDirtyRegion() const;

    QVector<QRect> takeDirtyRegion();

    void paintPolyline(const QVector <QPointF> &points,
                       int index = 0, int numPoints = -1);

    void paintLine(const KisPaintInformation &pi1,
                   const KisPaintInformation &pi2,
                   KisDistanceInformation *currentDistance);

    void paintBezierCurve(const KisPaintInformation &pi1,
                          const QPointF &control1,
                          const QPointF &control2,
                          const KisPaintInformation &pi2,
                          KisDistanceInformation *currentDistance);

    void getBezierCurvePoints(const QPointF &pos1,
                              const QPointF &control1,
                              const QPointF &control2,
                              const QPointF &pos2,
                              vQPointF& points) const;

    void paintRect(const QRectF &rect);

    void paintRect(const qreal x,
                   const qreal y,
                   const qreal w,
                   const qreal h);

    void paintEllipse(const QRectF &rect);

    void paintEllipse(const qreal x,
                      const qreal y,
                      const qreal w,
                      const qreal h);

    void paintPolygon(const vQPointF& points);

    void paintAt(const KisPaintInformation &pos,
                 KisDistanceInformation *savedDist);

    void paintPainterPath(const QPainterPath& path);

    void fillPainterPath(const QPainterPath& path);

    void fillPainterPath(const QPainterPath& path, const QRect &requestedRect);

    void drawPainterPath(const QPainterPath& path, const QPen& pen, const QRect &requestedRect);

    void drawPainterPath(const QPainterPath& path, const QPen& pen);

    void drawLine(const QPointF & start, const QPointF & end);

    void drawLine(const QPointF &start, const QPointF &end, qreal width, bool antialias);

    void drawDDALine(const QPointF & start, const QPointF & end);

    void drawWobblyLine(const QPointF & start, const QPointF & end);

    void drawWuLine(const QPointF & start, const QPointF & end);

    void drawThickLine(const QPointF & start, const QPointF & end, int startWidth, int endWidth);

    void setChannelFlags(QBitArray channelFlags);

    QBitArray channelFlags();

    void setPaintOpPreset(KisPaintOpPresetSP preset, KisNodeSP node, KisImageSP image);

    KisPaintOpPresetSP preset() const;

    KisPaintOp* paintOp() const;

    void setMirrorInformation(const QPointF &axesCenter, bool mirrorHorizontally, bool mirrorVertically);

    void copyMirrorInformationFrom(const KisPainter *other);

    bool hasMirroring() const;

    bool hasHorizontalMirroring() const;

    bool hasVerticalMirroring() const;

    void mirrorRect(Qt::Orientation direction, QRect *rc) const;

    void mirrorDab(Qt::Orientation direction, KisRenderedDab *dab, bool skipMirrorPixels = false) const;

    const QVector<QRect> calculateAllMirroredRects(const QRect &rc);

    const QVector<QPointF> calculateAllMirroredPoints(const QPointF &pos);

    const QVector<QPair<QPointF, QPointF>> calculateAllMirroredPoints(const QPair<QPointF, QPointF> &pair);

    void setPattern(const KoPatternSP pattern);

    const KoPatternSP pattern() const;

    void setPaintColor(const KoColor& color);

    const KoColor &paintColor() const;

    void setBackgroundColor(const KoColor& color);

    const KoColor &backgroundColor() const;

    void setGenerator(KisFilterConfigurationSP generator);

    const KisFilterConfigurationSP  generator() const;

    enum FillStyle {
        FillStyleNone,
        FillStyleForegroundColor,
        FillStyleBackgroundColor,
        FillStylePattern,
        FillStyleGenerator
    };

    void setFillStyle(FillStyle fillStyle);

    FillStyle fillStyle() const;

    void setPatternTransform(QTransform transform);

    QTransform patternTransform();

    void setAntiAliasPolygonFill(bool antiAliasPolygonFill);

    bool antiAliasPolygonFill();

    enum StrokeStyle {
        StrokeStyleNone,
        StrokeStyleBrush
    };

    void setStrokeStyle(StrokeStyle strokeStyle);

    StrokeStyle strokeStyle() const;

    void setFlow(qreal flow);

    qreal flow() const;

    void setOpacityUpdateAverage(qreal opacity);

    void setAverageOpacity(qreal averageOpacity);

    static qreal blendAverageOpacity(qreal opacity, qreal averageOpacity);

    void setOpacityU8(quint8 opacity);
    void setOpacityF(qreal opacity);

    qreal opacityF() const;

    bool isOpacityUnit() const;
    void setOpacityToUnit();

    void setCompositeOpId(const KoCompositeOp * op);

    QString compositeOpId();

    void setCompositeOpId(const QString& op);

    void addDirtyRect(const QRect &r);

    void addDirtyRects(const QVector<QRect> &rects);

    void setSelection(KisSelectionSP selection);

    KisSelectionSP selection();

    void setGradient(const KoAbstractGradientSP gradient);
    const KoAbstractGradientSP gradient() const;

    void setMaskImageSize(qint32 width, qint32 height);

    void setRenderingIntent(KoColorConversionTransformation::Intent intent);

    void setColorConversionFlags(KoColorConversionTransformation::ConversionFlags conversionFlags);

    void setRunnableStrokeJobsInterface(KisRunnableStrokeJobsInterface *interface);

    KisRunnableStrokeJobsInterface* runnableStrokeJobsInterface() const;

protected:
    void init();

    void fillPolygon(const vQPointF& points, FillStyle fillStyle);

private:

    KisPainter(const KisPainter&);
    KisPainter& operator=(const KisPainter&);

    float frac(float value) {
        float tmp = 0;
        return modff(value , &tmp);
    }

    float invertFrac(float value) {
        float tmp = 0;
        return 1.0f - modff(value , &tmp);
    }

protected:
    KoUpdater * progressUpdater();

private:
    template <bool useOldSrcData>
        void bitBltImpl(qint32 dstX, qint32 dstY,
                        const KisPaintDeviceSP srcDev,
                        qint32 srcX, qint32 srcY,
                        qint32 srcWidth, qint32 srcHeight);

    inline void compositeOnePixel(quint8 *dst, const KoColor &color);

private:

    struct Private;
    Private* const d;
};

#endif

