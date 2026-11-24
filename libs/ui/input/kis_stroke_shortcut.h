/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_STROKE_SHORTCUT_H
#define __KIS_STROKE_SHORTCUT_H

#include "kis_abstract_shortcut.h"

class QMouseEvent;
class QPointF;

class KRITAUI_EXPORT KisStrokeShortcut : public KisAbstractShortcut
{
public:
    KisStrokeShortcut(KisAbstractInputAction *action, int index);
    ~KisStrokeShortcut() override;

    int priority() const override;

    void setButtons(const QSet<Qt::Key> &modifiers,
                    const QSet<Qt::MouseButton> &buttons);

    bool matchReady(const QSet<Qt::Key> &modifiers,
                    const QSet<Qt::MouseButton> &buttons);

    bool matchBegin(Qt::MouseButton button);

    QMouseEvent fakeEndEvent(const QPointF &localPos) const;

private:
    class Private;
    Private * const m_d;
};

#endif
