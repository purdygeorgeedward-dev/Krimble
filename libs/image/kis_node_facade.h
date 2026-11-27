/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_NODE_FACADE_H
#define _KIS_NODE_FACADE_H

#include <QScopedPointer>

#include "kis_types.h"
#include "kis_node.h"
#include "kritaimage_export.h"

#include "KisNodeAdditionFlags.h"

class KRITAIMAGE_EXPORT KisNodeFacade
{

public:

    KisNodeFacade();

    KisNodeFacade(KisNodeSP root);

    virtual ~KisNodeFacade();

    void setRoot(KisNodeSP root);

    const KisNodeSP root() const;

    bool moveNode(KisNodeSP node, KisNodeSP parent, KisNodeSP aboveThis);

    bool moveNode(KisNodeSP node, KisNodeSP parent, quint32 index);
    bool addNode(KisNodeSP node, KisNodeSP parent = KisNodeSP(), KisNodeAdditionFlags flags = KisNodeAdditionFlag::None);

    bool addNode(KisNodeSP node, KisNodeSP parent, KisNodeSP aboveThis, KisNodeAdditionFlags flags = KisNodeAdditionFlag::None);

    bool addNode(KisNodeSP node,  KisNodeSP parent, quint32 index, KisNodeAdditionFlags flags = KisNodeAdditionFlag::None);

    bool removeNode(KisNodeSP node);

private:

    struct Private;
    QScopedPointer<Private> m_d;
};
#endif
