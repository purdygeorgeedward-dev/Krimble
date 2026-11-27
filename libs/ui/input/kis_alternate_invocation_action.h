/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ALTERNATE_INVOCATION_ACTION_H
#define KIS_ALTERNATE_INVOCATION_ACTION_H

#include "kis_abstract_input_action.h"
#include <QScopedPointer>
#include "kis_tool.h"

class KisAlternateInvocationAction : public KisAbstractInputAction
{
public:
    enum Shortcut {
        PrimaryAlternateModeShortcut,
        SecondaryAlternateModeShortcut,
        SampleColorFgLayerModeShortcut,
        SampleColorBgLayerModeShortcut,
        SampleColorFgImageModeShortcut,
        SampleColorBgImageModeShortcut,
        TertiaryAlternateModeShortcut
    };

    explicit KisAlternateInvocationAction();
    ~KisAlternateInvocationAction() override;

    void activate(int shortcut) override;
    void deactivate(int shortcut) override;

    int priority() const override;

    void begin(int shortcut, QEvent *event) override;
    void end(QEvent *event) override;
    void inputEvent(QEvent* event) override;

    bool supportsHiResInputEvents(int shortcut) const override;

private:
    KisTool::ToolAction shortcutToToolAction(int shortcut) const;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
