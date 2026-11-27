/*
 *  SPDX-FileCopyrightText: 2018 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISIMAGESIGNALS_H
#define KISIMAGESIGNALS_H

#include <QVector>
#include <QRectF>
#include "kritaimage_export.h"
#include "kis_types.h"

enum KisImageSignalTypeEnum {
    LayersChangedSignal,
    ModifiedWithoutUndoSignal,
    SizeChangedSignal,
    ProfileChangedSignal,
    ColorSpaceChangedSignal,
    ResolutionChangedSignal,
    NodeReselectionRequestSignal
};

struct KRITAIMAGE_EXPORT ComplexSizeChangedSignal {
    ComplexSizeChangedSignal();
    ComplexSizeChangedSignal(QPointF _oldStillPoint, QPointF _newStillPoint);

    ComplexSizeChangedSignal(const QRect &portionOfOldImage, const QSize &transformedIntoImageOfSize);

    ComplexSizeChangedSignal inverted() const;

    QPointF oldStillPoint;
    QPointF newStillPoint;
};

struct KRITAIMAGE_EXPORT ComplexNodeReselectionSignal {
    ComplexNodeReselectionSignal();
    ComplexNodeReselectionSignal(KisNodeSP _newActiveNode, KisNodeList _newSelectedNodes,
                                 KisNodeSP _oldActiveNode = KisNodeSP(), KisNodeList _oldSelectedNodes = KisNodeList());

    ComplexNodeReselectionSignal inverted() const;

    KisNodeSP newActiveNode;
    KisNodeList newSelectedNodes;
    KisNodeSP oldActiveNode;
    KisNodeList oldSelectedNodes;
};

struct KRITAIMAGE_EXPORT KisImageSignalType {
    KisImageSignalType();
    KisImageSignalType(KisImageSignalTypeEnum _id);
    KisImageSignalType(ComplexSizeChangedSignal signal);
    KisImageSignalType(ComplexNodeReselectionSignal signal);

    KisImageSignalType inverted() const;

    KisImageSignalTypeEnum id;
    ComplexSizeChangedSignal sizeChangedSignal;
    ComplexNodeReselectionSignal nodeReselectionSignal;
};

typedef QVector<KisImageSignalType> KisImageSignalVector;

#endif
