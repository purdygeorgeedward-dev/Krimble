/*
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_NODE_VISITOR_H_
#define KIS_NODE_VISITOR_H_

#include "kritaimage_export.h"

class KisNode;
class KisPaintLayer;
class KisGroupLayer;
class KisAdjustmentLayer;
class KisExternalLayer;
class KisCloneLayer;
class KisFilterMask;
class KisTransparencyMask;
class KisSelectionMask;
class KisGeneratorLayer;
class KisTransformMask;
class KisColorizeMask;

class KRITAIMAGE_EXPORT KisNodeVisitor
{
public:
    KisNodeVisitor() {}
    virtual ~KisNodeVisitor() {}

public:
    virtual bool visit(KisNode *node) = 0;

    virtual bool visit(KisPaintLayer *layer) = 0;

    virtual bool visit(KisGroupLayer *layer) = 0;

    virtual bool visit(KisAdjustmentLayer *layer) = 0;

    virtual bool visit(KisExternalLayer *layer) = 0;

    virtual bool visit(KisGeneratorLayer *layer) = 0;

    virtual bool visit(KisCloneLayer *layer) = 0;

    virtual bool visit(KisFilterMask *mask) = 0;

    virtual bool visit(KisTransformMask *mask) = 0;

    virtual bool visit(KisTransparencyMask *mask) = 0;

    virtual bool visit(KisSelectionMask *mask) = 0;

    virtual bool visit(KisColorizeMask *mask) = 0;

protected:

    bool visitAll(KisNode * node, bool breakOnFail = false);

    bool visitAllInverse(KisNode * node, bool breakOnFail = false);
};

#endif

