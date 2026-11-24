/*
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_PRESCALED_PROJECTION_H
#define KIS_PRESCALED_PROJECTION_H

#include <QObject>

#include <kritaui_export.h>
#include <kis_shared.h>

#include "KoColorConversionTransformation.h"
class QImage;
class QRect;
class QSize;
class QPainter;

class KoColorProfile;
class KisCoordinatesConverter;
class KisDisplayFilter;
class KisDisplayConfig;
class KisCanvasState;

#include <kis_types.h>
#include "kis_ui_types.h"

class KRITAUI_EXPORT KisPrescaledProjection : public QObject, public KisShared
{
    Q_OBJECT
public:

    KisPrescaledProjection();
    ~KisPrescaledProjection() override;

    void setImage(KisImageWSP image);

    QImage prescaledQImage() const;

    void setCoordinatesConverter(KisCoordinatesConverter *coordinatesConverter);

public Q_SLOTS:

    KisUpdateInfoSP updateCache(const QRect &dirtyImageRect);

    void recalculateCache(KisUpdateInfoSP info);

    void updateSettings();

    void notifyCanvasStateChanged(const KisCanvasState &state);

    void slotImageSizeChanged(qint32 w, qint32 h);

    void notifyCanvasSizeChanged(const QSize &widgetSize);

    void setDisplayConfig(const KisDisplayConfig &config);

    void setChannelFlags(const QBitArray &channelFlags);

    void setDisplayFilter(QSharedPointer<KisDisplayFilter> displayFilter);

    void preScale();

private:

    friend class KisPrescaledProjectionTest;

    KisPrescaledProjection(const KisPrescaledProjection &);
    KisPrescaledProjection operator=(const KisPrescaledProjection &);

    void updateViewportSize();

    void viewportMoved(const QPointF &offset);

    KisPPUpdateInfoSP getInitialUpdateInformation(const QRect &dirtyImageRect);

    void fillInUpdateInformation(const QRect &viewportRect,
                                 KisPPUpdateInfoSP info);

    void updateScaledImage(KisPPUpdateInfoSP info);

    void drawUsingBackend(QPainter &gc, KisPPUpdateInfoSP info);

    struct Private;
    Private * const m_d;
};

#endif
