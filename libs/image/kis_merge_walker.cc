/*
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_merge_walker.h"
#include "kis_projection_leaf.h"

KisMergeWalker::KisMergeWalker(QRect cropRect,  Flags flags)
    : m_flags(flags)
{
    setCropRect(cropRect);
    setClonesDontInvalidateFrames(flags.testFlag(CLONES_DONT_INVALIDATE_FRAMES));
}

KisMergeWalker::~KisMergeWalker()
{
}

KisBaseRectsWalker::UpdateType KisMergeWalker::type() const
{
    return !m_flags.testFlag(NO_FILTHY) ? KisBaseRectsWalker::UPDATE : KisBaseRectsWalker::UPDATE_NO_FILTHY;
}

void KisMergeWalker::startTripImpl(KisProjectionLeafSP startLeaf, KisMergeWalker::Flags flags)
{
    if(startLeaf->isMask()) {
        startTripWithMask(startLeaf, flags);
        return;
    }

    visitHigherNode(startLeaf,
                    !flags.testFlag(NO_FILTHY) ? N_FILTHY : N_ABOVE_FILTHY);

    KisProjectionLeafSP prevLeaf = startLeaf->prevSibling();
    if(prevLeaf)
        visitLowerNode(prevLeaf);
}

void KisMergeWalker::startTrip(KisProjectionLeafSP startLeaf)
{
    startTripImpl(startLeaf, m_flags);
}

void KisMergeWalker::startTripWithMask(KisProjectionLeafSP filthyMask, KisMergeWalker::Flags flags)
{
    KisProjectionLeafSP parentLayer = filthyMask->parent();
    if (!parentLayer) {
        return;
    }

    adjustMasksChangeRect(filthyMask);

    KisProjectionLeafSP nextLeaf = parentLayer->nextSibling();
    KisProjectionLeafSP prevLeaf = parentLayer->prevSibling();

    if (nextLeaf)
        visitHigherNode(nextLeaf, N_ABOVE_FILTHY);
    else if (parentLayer->parent())
        startTripImpl(parentLayer->parent(), DEFAULT);

    NodePosition positionToFilthy =
        (!flags.testFlag(NO_FILTHY) ? N_FILTHY_PROJECTION : N_ABOVE_FILTHY) |
        calculateNodePosition(parentLayer);
    registerNeedRect(parentLayer, positionToFilthy, KisRenderPassFlag::None);

    if(prevLeaf)
        visitLowerNode(prevLeaf);
}

void KisMergeWalker::visitHigherNode(KisProjectionLeafSP leaf, NodePosition positionToFilthy)
{
    positionToFilthy |= calculateNodePosition(leaf);

    registerChangeRect(leaf, positionToFilthy);

    KisProjectionLeafSP nextLeaf = leaf->nextSibling();
    if (nextLeaf)
        visitHigherNode(nextLeaf, N_ABOVE_FILTHY);
    else if (leaf->parent())
        startTripImpl(leaf->parent(), DEFAULT);

    registerNeedRect(leaf, positionToFilthy, KisRenderPassFlag::None);
}

void KisMergeWalker::visitLowerNode(KisProjectionLeafSP leaf)
{
    NodePosition position =
        N_BELOW_FILTHY | calculateNodePosition(leaf);
    registerNeedRect(leaf, position, KisRenderPassFlag::None);

    KisProjectionLeafSP prevLeaf = leaf->prevSibling();
    if (prevLeaf)
        visitLowerNode(prevLeaf);
}
