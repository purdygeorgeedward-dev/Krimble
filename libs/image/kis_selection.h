/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_SELECTION_H_
#define KIS_SELECTION_H_

#include <QRect>

#include "kis_types.h"
#include "kritaimage_export.h"

#include "KisSelectionTags.h"

#include "kis_pixel_selection.h"

class KisSelectionComponent;
class QPainterPath;

class KRITAIMAGE_EXPORT KisSelection : public KisShared
{
private:
    struct ChangeShapeSelectionCommand;

public:

    KisSelection();

    KisSelection(KisDefaultBoundsBaseSP defaultBounds, KisImageResolutionProxySP resolutionProxy);

    KisSelection(const KisSelection& rhs);
    KisSelection& operator=(const KisSelection &rhs);

    virtual ~KisSelection();

    KisSelection(const KisPaintDeviceSP copySource, KritaUtils::DeviceCopyMode copyMode, KisDefaultBoundsBaseSP defaultBounds, KisImageResolutionProxySP resolutionProxy);

    void setParentNode(KisNodeWSP node);

    bool hasNonEmptyPixelSelection() const;
    bool hasNonEmptyShapeSelection() const;
    bool hasShapeSelection() const;

    bool outlineCacheValid() const;
    QPainterPath outlineCache() const;
    void recalculateOutlineCache();

    bool thumbnailImageValid() const;

    void recalculateThumbnailImage(const QColor &maskColor);

    QImage thumbnailImage() const;

    QTransform thumbnailImageTransform() const;

    KisPixelSelectionSP pixelSelection() const;

    KisSelectionComponent* shapeSelection() const;

    KUndo2Command* convertToVectorSelection(KisSelectionComponent* shapeSelection);

    void convertToVectorSelectionNoUndo(KisSelectionComponent* shapeSelection);

    KisPixelSelectionSP projection() const;

    void updateProjection(const QRect& rect);
    void updateProjection();

    void setVisible(bool visible);
    bool isVisible();

    bool isTotallyUnselected(const QRect & r) const;

    QRect selectedRect() const;

    QRect selectedExactRect() const;

    void setX(qint32 x);
    void setY(qint32 y);

    qint32 x() const;
    qint32 y() const;

    void setDefaultBounds(KisDefaultBoundsBaseSP bounds);
    void setResolutionProxy(KisImageResolutionProxySP proxy);
    KisImageResolutionProxySP resolutionProxy() const;

    void clear();

    KUndo2Command* flatten();

    void notifySelectionChanged();

    void requestCompressedProjectionUpdate(const QRect &rc);

    quint8 selected(qint32 x, qint32 y) const;

    KisNodeWSP parentNode() const;

private:
    friend class KisSelectionTest;
    friend class KisMaskTest;
    friend class KisAdjustmentLayerTest;
    friend class KisUpdateSelectionJob;
    friend class KisSelectionUpdateCompressor;
    friend class KisDeselectActiveSelectionCommand;

    void copyFrom(const KisSelection &rhs);

private:

    struct Private;
    Private * const m_d;
};

#endif
