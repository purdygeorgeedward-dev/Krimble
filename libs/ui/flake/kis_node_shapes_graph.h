/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_NODE_SHAPES_GRAPH_H
#define __KIS_NODE_SHAPES_GRAPH_H

#include "kis_node.h"
#include "kis_types.h"
#include "kis_node_dummies_graph.h"

class KisNodeShape;
class KisNodeDummy;

class KRITAUI_EXPORT KisNodeShapesGraph
{
public:
    KisNodeShape* addNode(KisNodeSP node, KisNodeSP parent, KisNodeSP aboveThis);

    void moveNode(KisNodeSP node, KisNodeSP parent, KisNodeSP aboveThis);

    void removeNode(KisNodeSP node);

    KisNodeShape* nodeToShape(KisNodeSP node);
    KisNodeDummy* nodeToDummy(KisNodeSP node);
    KisNodeDummy* rootDummy() const;

    bool containsNode(KisNodeSP node) const;
    int shapesCount() const;

private:
    void unmapDummyRecursively(KisNodeDummy *dummy);

private:
    KisNodeDummiesGraph m_dummiesGraph;
};

#endif
