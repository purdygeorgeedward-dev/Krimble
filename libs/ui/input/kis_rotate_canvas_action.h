/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ROTATE_CANVAS_ACTION_H
#define KIS_ROTATE_CANVAS_ACTION_H

#include "kis_abstract_input_action.h"

class KisRotateCanvasAction : public KisAbstractInputAction
{

public:
    enum Shortcut {
        RotateModeShortcut,
        DiscreteRotateModeShortcut,
        RotateLeftShortcut,
        RotateRightShortcut,
        RotateResetShortcut
    };
    explicit KisRotateCanvasAction();
    ~KisRotateCanvasAction() override;

    int priority() const override;

    void activate(int shortcut) override;
    void deactivate(int shortcut) override;
    void begin(int shortcut, QEvent *event) override;
    void end(QEvent *event) override;
    void cursorMovedAbsolute(const QPointF &startPos, const QPointF &pos) override;
    void inputEvent(QEvent* event) override;

    KisInputActionGroup inputActionGroup(int shortcut) const override;
    bool supportsHiResInputEvents(int shortcut) const override;

private:
    class Private;
    Private * const d {nullptr};
};

#endif
