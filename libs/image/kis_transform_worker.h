/*
 *  SPDX-FileCopyrightText: 2004 Michael Thaler <michael.thaler@physik.tu-muenchen.de>
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2010 Marc Pegon <pe.marc@free.fr>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TRANSFORM_WORKER_H_
#define KIS_TRANSFORM_WORKER_H_

#include "kis_types.h"
#include "kritaimage_export.h"

#include <QRect>
#include <KoUpdater.h>

class KisPaintDevice;
class KisFilterStrategy;
class QTransform;

class KRITAIMAGE_EXPORT KisTransformWorker
{

public:
    KisTransformWorker(KisPaintDeviceSP dev,
                       double  xscale, double  yscale,
                       double  xshear, double  yshear,
                       double rotation,
                       qreal xtranslate, qreal ytranslate,
                       KoUpdaterPtr progress,
                       KisFilterStrategy *filter);
    ~KisTransformWorker();

    static void mirror(KisPaintDeviceSP dev, qreal axis, Qt::Orientation orientation);

    static void mirrorX(KisPaintDeviceSP dev, qreal axis);
    static void mirrorY(KisPaintDeviceSP dev, qreal axis);

    static void mirrorX(KisPaintDeviceSP dev);
    static void mirrorY(KisPaintDeviceSP dev);

    static void offset(KisPaintDeviceSP device, const QPoint &offsetPosition, const QRect &wrapRect);

public:

    bool run();
    bool runPartial(const QRect &processRect);

    QTransform transform() const;

    void transformPixelSelectionOutline(KisPixelSelectionSP pixelSelection) const;

    bool forceSubPixelTranslation() const;
    void setForceSubPixelTranslation(bool value);

private:
    template <class T> void transformPass(KisPaintDevice* src,
                                          KisPaintDevice* dst,
                                          double xscale,
                                          double  shear,
                                          double dx,
                                          KisFilterStrategy *filterStrategy,
                                          int portion);

    friend class KisTransformWorkerTest;

    static QRect rotateRight90(KisPaintDeviceSP dev,
                               QRect boundRect,
                               KoUpdaterPtr progressUpdater,
                               int portion);

    static QRect rotateLeft90(KisPaintDeviceSP dev,
                              QRect boundRect,
                              KoUpdaterPtr progressUpdater,
                              int portion);

    static QRect rotate180(KisPaintDeviceSP dev,
                           QRect boundRect,
                           KoUpdaterPtr progressUpdater,
                           int portion);

private:
    KisPaintDeviceSP m_dev;
    double  m_xscale, m_yscale;
    double  m_xshear, m_yshear, m_rotation;
    double  m_xshearOrigin, m_yshearOrigin;
    qreal  m_xtranslate, m_ytranslate;
    KoUpdaterPtr m_progressUpdater;
    KisFilterStrategy *m_filter;
    QRect m_boundRect;
    bool m_forceSubPixelTranslation {false};
};

#endif
