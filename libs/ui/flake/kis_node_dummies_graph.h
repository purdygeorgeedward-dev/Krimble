/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_NODE_DUMMIES_GRAPH_H
#define __KIS_NODE_DUMMIES_GRAPH_H

#include <QList>
#include <QMap>

#include "kritaui_export.h"
#include "kis_types.h"
#include "kis_node.h"

class KisNodeShape;

class KRITAUI_EXPORT KisNodeDummy : public QObject
{
    Q_OBJECT

public:
    KisNodeDummy(KisNodeShape *nodeShape, KisNodeSP node);
    ~KisNodeDummy() override;

    KisNodeDummy* firstChild() const;
    KisNodeDummy* lastChild() const;
    KisNodeDummy* nextSibling() const;
    KisNodeDummy* prevSibling() const;
    KisNodeDummy* parent() const;

    KisNodeDummy* at(int index) const;
    int childCount() const;
    int indexOf(KisNodeDummy *child) const;

    KisNodeSP node() const;

    bool isGUIVisible(bool showGlobalSelection) const;

private:
    friend class KisNodeShapesGraph;
    friend class KisNodeShapesGraphTest;
    KisNodeShape* nodeShape() const;

    friend class KisNodeDummiesGraph;
    QList<KisNodeDummy*> m_children;

    KisNodeShape *m_nodeShape;
    KisNodeSP m_node;
};

class KRITAUI_EXPORT KisNodeDummiesGraph
{
public:
    KisNodeDummiesGraph();

    KisNodeDummy* rootDummy() const;

    KisNodeDummy* nodeToDummy(KisNodeSP node);
    bool containsNode(KisNodeSP node) const;
    int dummiesCount() const;

    void addNode(KisNodeDummy *node, KisNodeDummy *parent, KisNodeDummy *aboveThis);

    void moveNode(KisNodeDummy *node, KisNodeDummy *parent, KisNodeDummy *aboveThis);

    void removeNode(KisNodeDummy *node);

private:
    void unmapDummyRecursively(KisNodeDummy *dummy);

private:
    typedef QMap<KisNodeSP, KisNodeDummy*> NodeMap;

private:
    KisNodeDummy *m_rootDummy;
    NodeMap m_dummiesMap;
};

#endif
