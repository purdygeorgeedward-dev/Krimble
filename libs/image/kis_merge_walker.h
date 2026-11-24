/*
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_MERGE_WALKER_H
#define __KIS_MERGE_WALKER_H

#include "kis_types.h"
#include "kis_base_rects_walker.h"

class KisMergeWalker;
typedef KisSharedPtr<KisMergeWalker> KisMergeWalkerSP;

class KRITAIMAGE_EXPORT KisMergeWalker : public virtual KisBaseRectsWalker
{

public:
    enum Flag {
        DEFAULT = 0,
        NO_FILTHY = 0x1,
        CLONES_DONT_INVALIDATE_FRAMES = 0x2
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    KisMergeWalker(QRect cropRect, Flags flags = DEFAULT);

    ~KisMergeWalker() override;

    UpdateType type() const override;

protected:
    KisMergeWalker() : m_flags(DEFAULT) {}
    KisMergeWalker(Flags flags) : m_flags(flags) {}

    void startTrip(KisProjectionLeafSP startWith) override;

    using KisBaseRectsWalker::startTrip;

    void startTripWithMask(KisProjectionLeafSP filthyMask, KisMergeWalker::Flags flags);

private:
    void startTripImpl(KisProjectionLeafSP startLeaf, Flags flags);

private:
    void visitHigherNode(KisProjectionLeafSP leaf, NodePosition positionToFilthy);

    void visitLowerNode(KisProjectionLeafSP leaf);

private:
    const Flags m_flags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisMergeWalker::Flags)

#endif

