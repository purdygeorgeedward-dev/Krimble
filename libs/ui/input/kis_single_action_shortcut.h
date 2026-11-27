/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SINGLE_ACTION_SHORTCUT_H
#define __KIS_SINGLE_ACTION_SHORTCUT_H

#include "kis_abstract_shortcut.h"

class KRITAUI_EXPORT KisSingleActionShortcut : public KisAbstractShortcut
{
public:
    enum WheelAction {
        WheelUp,
        WheelDown,
        WheelLeft,
        WheelRight,
        WheelTrackpad,
    };

    KisSingleActionShortcut(KisAbstractInputAction *action, int index);
    ~KisSingleActionShortcut() override;

    int priority() const override;

    void setKey(const QSet<Qt::Key> &modifiers, Qt::Key key);
    void setWheel(const QSet<Qt::Key> &modifiers, WheelAction wheelAction);

    bool match(const QSet<Qt::Key> &modifiers, Qt::Key key);
    bool match(const QSet<Qt::Key> &modifiers, WheelAction wheelAction);

    bool conflictsWith(const QKeySequence &seq);

private:
    class Private;
    Private * const m_d;
};

#endif
