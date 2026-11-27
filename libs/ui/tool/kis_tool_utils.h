/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2018 Emmet & Eoin O'Neill <emmetoneill.pdx@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_UTILS_H
#define KIS_TOOL_UTILS_H

#include <kis_types.h>
#include <kritaui_export.h>

class QPoint;
class KoColor;
class KoCanvasBase;
class QPainterPath;

namespace KisToolUtils {

struct KRITAUI_EXPORT ColorSamplerConfig {
    ColorSamplerConfig();

    bool toForegroundColor;
    bool updateColor;
    bool addColorToCurrentPalette;
    bool normaliseValues;
    bool sampleMerged;
    int radius;
    int blend;

    void save() const;
    void load();
private:
    static const QString CONFIG_GROUP_NAME;
};

bool KRITAUI_EXPORT sampleColor(KoColor &out_color, KisPaintDeviceSP dev, const QPoint &pos,
                                KoColor const *const blendColor = nullptr, int radius = 1,
                                int blend = 100, bool pure = false);

KisNodeSP KRITAUI_EXPORT findNode(KisNodeSP node, const QPoint &point, bool wholeGroup, bool editableOnly = true);

KisNodeList KRITAUI_EXPORT findNodes(KisNodeSP node, const QPoint &point, bool wholeGroup,
                                     bool includeGroups = true, bool editableOnly = true);

QString KRITAUI_EXPORT nodeEditableMessage(KisNodeSP node, bool blockedNoIndirectPainting = false);

QPainterPath KRITAUI_EXPORT shapeHoverInfoCrossLayer(KoCanvasBase *canvas, const QPointF &point,
                                                     QString &shapeType, bool *isHorizontal = nullptr, bool skipCurrentShapes = true);
bool KRITAUI_EXPORT selectShapeCrossLayer(KoCanvasBase *canvas, const QPointF &point,
                                          const QString &shapeType = QString(), bool skipCurrentShapes = true);

bool KRITAUI_EXPORT clearImage(KisImageSP image, KisNodeList nodes, KisSelectionSP selection);

void KRITAUI_EXPORT setCursorPos(const QPoint &point);

void KRITAUI_EXPORT showBrushSizeFloatingMessage(KoCanvasBase *canvas, qreal size);

class KRITAUI_EXPORT StandardBrushSizes {
public:
    StandardBrushSizes(int minSize, int maxSize);

    int increaseBrushSize(qreal size);
    int decreaseBrushSize(qreal size);

private:
    std::vector<int> m_sizes;
};
}

#endif
