/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISCHANGEPRIMARYSETTINGACTION_H
#define KISCHANGEPRIMARYSETTINGACTION_H

#include "kis_abstract_input_action.h"
#include "kis_tool.h"

class KisChangePrimarySettingAction : public KisAbstractInputAction
{
public:
    explicit KisChangePrimarySettingAction();
    ~KisChangePrimarySettingAction() override;

    enum Shortcut {
        PrimaryAlternateChangeSizeShortcut,
        SecondaryAlternateChangeSizeShortcut,
    };

    void activate(int shortcut) override;
    void deactivate(int shortcut) override;
    int priority() const override;

    void begin(int shortcut, QEvent *event) override;
    void end(QEvent *event) override;
    void inputEvent(QEvent* event) override;

    bool supportsHiResInputEvents(int shortcut) const override;
private:
    KisTool::ToolAction decodeAction(int shortcut) const;
    KisTool::ToolAction savedAction {KisTool::Alternate_NONE};
};

#endif
