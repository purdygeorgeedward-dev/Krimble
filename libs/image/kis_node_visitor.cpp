/*
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2007, 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_node_visitor.h"
#include "kis_node.h"

bool KisNodeVisitor::visitAll(KisNode * node, bool breakOnFail)
{
    for (uint i = 0; i < node->childCount(); ++i) {
        if (!node->at(i)->accept(*this)) {
            if (breakOnFail)
                return false;
        }
    }
    return true;
}

bool KisNodeVisitor::visitAllInverse(KisNode * node, bool breakOnFail)
{
    KisNodeSP child = node->lastChild();
    while (child) {
        if (!child->accept(*this)) {
            if (breakOnFail)
                return false;
        }
        child = child->prevSibling();
    }
    return true;
}
