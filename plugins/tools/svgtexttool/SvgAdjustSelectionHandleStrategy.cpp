/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "SvgAdjustSelectionHandleStrategy.h"
#include "SvgTextCursor.h"

SvgAdjustSelectionHandleStrategy::SvgAdjustSelectionHandleStrategy(KoToolBase *tool, SvgTextCursor *cursor, bool adjustingStartHandle)
    : KoInteractionStrategy(tool)
    , m_cursor(cursor)
    , m_adjustingStartHandle(adjustingStartHandle)
{
}

void SvgAdjustSelectionHandleStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    if (m_adjustingStartHandle) {
        m_cursor->setAnchorToPoint(mouseLocation);
    } else {
        m_cursor->setPosToPoint(mouseLocation, false);
    }
}

KUndo2Command *SvgAdjustSelectionHandleStrategy::createCommand()
{
    return nullptr;
}

void SvgAdjustSelectionHandleStrategy::cancelInteraction()
{
    return;
}

void SvgAdjustSelectionHandleStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
}
