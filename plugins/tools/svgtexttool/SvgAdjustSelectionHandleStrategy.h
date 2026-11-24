/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGADJUSTSELECTIONHANDLESTRATEGY_H
#define SVGADJUSTSELECTIONHANDLESTRATEGY_H

#include <KoInteractionStrategy.h>
#include <QPointF>

class SvgTextCursor;

class SvgAdjustSelectionHandleStrategy : public KoInteractionStrategy
{
public:
    SvgAdjustSelectionHandleStrategy(KoToolBase *tool, SvgTextCursor *cursor, bool adjustingStartHandle);
    ~SvgAdjustSelectionHandleStrategy() override = default;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
    void cancelInteraction() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

private:
    SvgTextCursor *m_cursor;
    bool m_adjustingStartHandle;
};

#endif
