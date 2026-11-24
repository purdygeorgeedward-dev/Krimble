/*
 *  SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ABSTRACT_SHORTCUT_H
#define __KIS_ABSTRACT_SHORTCUT_H

#include <Qt>
#include <QSet>
#include <kritaui_export.h>
#include "KisInputActionGroup.h"

class KisAbstractInputAction;

class KRITAUI_EXPORT KisAbstractShortcut
{
public:
    KisAbstractShortcut(KisAbstractInputAction *action, int index);
    virtual ~KisAbstractShortcut();

    virtual int priority() const = 0;

    KisAbstractInputAction* action() const;

    void setAction(KisAbstractInputAction *action);

    int shortcutIndex() const;

    bool isAvailable(KisInputActionGroupsMask mask) const;

protected:
    bool compareKeys(const QSet<Qt::Key> &keys1,
                     const QSet<Qt::Key> &keys2);

private:
    class Private;
    Private * const m_d;
};

#endif
