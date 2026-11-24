/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_IMAGE_PIPE_BRUSH_P_H
#define KIS_IMAGE_PIPE_BRUSH_P_H

#include "kis_imagepipe_brush.h"
#include <math.h>
#include <limits.h>

#include <QFile>
#include <QPoint>

#include <kis_debug.h>
#include <klocalizedstring.h>
#include <krandom.h>

#include "kis_global.h"
#include "kis_paint_device.h"
#include "kis_layer.h"

#include "kritabrush_export.h"

class BRUSH_EXPORT KisPipeBrushParasite
{
public:
    KisPipeBrushParasite()
        : ncells(0)
        , dim(0)
        , needsMovement(false) {
        init();
    }

    void init();
    void sanitize();

    void setBrushesCount();

    KisPipeBrushParasite(QStringView source);

    bool saveToDevice(QIODevice* dev) const;
    bool loadFromDevice(QIODevice *dev);

    enum Placement { DefaultPlacement, ConstantPlacement, RandomPlacement };

    static int const MaxDim = 4;

    qint32 ncells {0};
    qint32 dim {0};

    qint32 rank[MaxDim] {};

    KisParasite::SelectionMode selection[MaxDim];
    QString selectionMode;

    qint32 brushesCount[MaxDim];

    qint32 index[MaxDim];

    bool needsMovement {false};
};
#endif
