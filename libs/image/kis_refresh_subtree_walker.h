/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_REFRESH_SUBTREE_WALKER_H
#define __KIS_REFRESH_SUBTREE_WALKER_H

#include "kis_types.h"
#include "kis_base_rects_walker.h"

class KRITAIMAGE_EXPORT KisRefreshSubtreeWalker : public virtual KisBaseRectsWalker
{
public:
    enum Flag {
        None = 0x0,
        SkipNonRenderableNodes = 0x1,
        NoFilthyMode = 0x2,
        DontAdjustChangeRect = 0x4,
        ClonesDontInvalidateFrames = 0x8
    };

    Q_DECLARE_FLAGS(Flags, Flag);

public:
    KisRefreshSubtreeWalker(QRect cropRect, Flags flags = None)
        : m_flags(flags)
    {
        setCropRect(cropRect);
        setClonesDontInvalidateFrames(flags.testFlag(ClonesDontInvalidateFrames));
    }

    UpdateType type() const override {
        return UNSUPPORTED;
    }

    ~KisRefreshSubtreeWalker() override
    {
    }

    Flags flags() const {
        return m_flags;
    }

protected:
    KisRefreshSubtreeWalker() {}

    static
    std::pair<QRect, bool>
    calculateChangeRect(KisProjectionLeafSP startWith,
                        const QRect &requestedRect) {

        if(!startWith->isLayer())
            return std::make_pair(requestedRect, false);

        QRect finalChangeRect = requestedRect;
        bool changeRectVaries = false;

        KisProjectionLeafSP currentLeaf = startWith->firstChild();

        while (currentLeaf) {
            if (currentLeaf->isLayer() && currentLeaf->shouldBeRendered()) {
                QRect leafRect;

                std::tie(leafRect, changeRectVaries) =
                    calculateChangeRect(currentLeaf, requestedRect);

                finalChangeRect |= leafRect;
            }

            currentLeaf = currentLeaf->nextSibling();
        }

        finalChangeRect |= startWith->projectionPlane()->changeRect(finalChangeRect);

        if (!changeRectVaries) {
            changeRectVaries = finalChangeRect != requestedRect;
        }

        return std::make_pair(finalChangeRect, changeRectVaries);
    }

    void startTrip(KisProjectionLeafSP startWith) override {
        if (!m_flags.testFlag(DontAdjustChangeRect)) {
            setExplicitChangeRect(requestedRect(), false);
        }

        if (isStartLeaf(startWith)) {
            KisProjectionLeafSP extraUpdateLeaf = startWith;

            if (startWith->isMask()) {

                extraUpdateLeaf = startWith->parent();
            }

            if (extraUpdateLeaf) {
                NodePosition pos = N_EXTRA | calculateNodePosition(extraUpdateLeaf);
                registerNeedRect(extraUpdateLeaf, pos, KisRenderPassFlag::None);

                registerCloneNotification(extraUpdateLeaf->node(), pos);
            }
        }

        SubtreeVisitFlags subtreeFlags = SubtreeVisitFlag::None;
        if (m_flags & SkipNonRenderableNodes) {
            subtreeFlags |= SubtreeVisitFlag::SkipNonRenderableNodes;
        }
        if (m_flags & NoFilthyMode) {
            subtreeFlags |= SubtreeVisitFlag::NoFilthyMode;
        }

        visitSubtreeTopToBottom(startWith, subtreeFlags, KisRenderPassFlag::None, cropRect());
    }

private:
    Flags m_flags = None;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisRefreshSubtreeWalker::Flags);

#endif

