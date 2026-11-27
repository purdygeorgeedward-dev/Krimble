/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOINSETS_H
#define KOINSETS_H

#include "kritaflake_export.h"

#include <QDebug>

struct KRITAFLAKE_EXPORT KoInsets
{
public:
    KoInsets(qreal top, qreal left, qreal bottom, qreal right)
    : top(top)
    , bottom(bottom)
    , left(left)
    , right(right)
    {
    }
    KoInsets() : top(0.), bottom(0.), left(0.), right(0.) {
    }

    void clear() {
        top = 0;
        bottom = 0;
        left = 0;
        right = 0;
    }

    qreal top;
    qreal bottom;
    qreal left;
    qreal right;
};

#ifndef QT_NO_DEBUG_STREAM
KRITAFLAKE_EXPORT QDebug operator<<(QDebug, const KoInsets &);
#endif

#endif
