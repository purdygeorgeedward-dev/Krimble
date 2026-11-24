/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ZOOM_ACTION_H
#define KIS_ZOOM_ACTION_H

#include "kis_abstract_input_action.h"

class KisZoomAction : public KisAbstractInputAction
{
public:
    enum Shortcuts {
        ZoomModeShortcut,
        DiscreteZoomModeShortcut,
        ZoomInToCursorShortcut,
        ZoomOutFromCursorShortcut,
        Zoom100PctShortcut,
        FitToViewShortcut,
        FitToWidthShortcut,
        RelativeZoomModeShortcut,
        RelativeDiscreteZoomModeShortcut,
        FitToHeightShortcut,
        ZoomInShortcut,
        ZoomOutShortcut,
    };
    explicit KisZoomAction();
    ~KisZoomAction() override;

    int priority() const override;

    void activate(int shortcut) override;
    void deactivate(int shortcut) override;

    void begin(int shortcut, QEvent *event = 0) override;
    void inputEvent(QEvent* event) override;
    void cursorMovedAbsolute(const QPointF &startPos, const QPointF &pos) override;

    bool isShortcutRequired(int shortcut) const override;
    bool supportsHiResInputEvents(int shortcut) const override;

    KisInputActionGroup inputActionGroup(int shortcut) const override;

private:
    class Private;
    Private * const d {nullptr};
};

#endif
