/*
  *  SPDX-FileCopyrightText: 2017 Scott Petrovic <scottpetrovic@gmail.com>
  *
  *  SPDX-License-Identifier: GPL-2.0-or-later
  */

#ifndef _KIS_PRESET_LIVE_PREVIEW_
#define _KIS_PRESET_LIVE_PREVIEW_

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainterPath>
#include <QGraphicsPixmapItem>

#include "kis_paintop_preset.h"
#include "KoColorSpaceRegistry.h"
#include "kis_paint_layer.h"
#include "kis_painter.h"
#include "kis_distance_information.h"
#include "kis_painting_information_builder.h"
#include <kis_image.h>
#include <kis_types.h>
#include <KoColor.h>
#include "kis_signal_compressor.h"

class KoCanvasResourceProvider;

class KisPresetLivePreviewView : public QGraphicsView
{
    Q_OBJECT

public:

    KisPresetLivePreviewView(QWidget *parent);
    ~KisPresetLivePreviewView();

    void setup(KoCanvasResourceProvider* resourceManager);

    void setCurrentPreset(KisPaintOpPresetSP preset);
    void requestUpdateStroke();

private Q_SLOTS:
    void updateStroke();
    void slotPreviewGenerationCompleted();

private:

    KoCanvasResourceProvider* m_resourceManager {nullptr};

    KisImageSP m_image;

    KisLayerSP m_layer;

    const KoColorSpace *m_colorSpace {nullptr};

    KoColor m_paintColor;

    QGraphicsScene *m_brushPreviewScene {nullptr};

    QGraphicsPixmapItem *m_sceneImageItem {nullptr};

    QGraphicsTextItem *m_noPreviewText {nullptr};

    QRect m_canvasSize;

    QPointF m_canvasCenterPoint;

    KisDistanceInformation m_currentDistance;
    QPainterPath m_curvedLine;
    KisPaintInformation m_curvePointPI1;
    KisPaintInformation m_curvePointPI2;

    KisPaintOpPresetSP m_currentPreset;

    float m_currentBrushSize {1.0};

    bool m_previewGenerationInProgress {false};
    KisSignalCompressor m_updateCompressor;

    const float m_minBrushVal {10.0};
    const float m_maxBrushVal {100.0};

    const qreal m_minScale {1.0};
    const qreal m_maxScale {0.3};

    const float m_minStrokeScale {0.4};
    const float m_maxStrokeScale {1.0};

    void paintBackground();

    void setupAndPaintStroke();

    void changeEvent(QEvent*) override;
};

#endif
