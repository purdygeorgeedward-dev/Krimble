/*
 *  SPDX-FileCopyrightText: 2024 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISGRABKEYBOARDFOCUSRECOVERYWORKAROUND_H
#define KISGRABKEYBOARDFOCUSRECOVERYWORKAROUND_H

#include <QScopedPointer>

#include "kritaui_export.h"

class KRITAUI_EXPORT KisGrabKeyboardFocusRecoveryWorkaround
{
public:
    static KisGrabKeyboardFocusRecoveryWorkaround* instance();

    void recoverFocus();

private:
    class Private;
    QScopedPointer<Private> m_d;

    KisGrabKeyboardFocusRecoveryWorkaround();
    Q_DISABLE_COPY(KisGrabKeyboardFocusRecoveryWorkaround)
};

#endif
