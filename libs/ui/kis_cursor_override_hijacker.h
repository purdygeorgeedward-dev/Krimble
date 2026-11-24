/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_CURSOR_OVERRIDE_HIJACKER_H
#define __KIS_CURSOR_OVERRIDE_HIJACKER_H

#include <QCursor>
#include <QStack>
#include <QApplication>

class KisCursorOverrideHijacker
{
public:
    KisCursorOverrideHijacker() {
        while (qApp->overrideCursor()) {
            m_cursorStack.push(*qApp->overrideCursor());
            qApp->restoreOverrideCursor();
        }
    }

    ~KisCursorOverrideHijacker() {
        while (!m_cursorStack.isEmpty()) {
            qApp->setOverrideCursor(m_cursorStack.pop());
        }
    }

private:
    QStack<QCursor> m_cursorStack;
};

#endif
