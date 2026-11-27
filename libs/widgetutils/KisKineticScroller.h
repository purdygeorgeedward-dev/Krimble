/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Emmet O 'Neill <emmetoneill.pdx@gmail.com>
 * SPDX-FileCopyrightText: 2018 Eoin O 'Neill <eoinoneill1991@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISKINECTICSCROLLER_H
#define KISKINECTICSCROLLER_H
#include <kritawidgetutils_export.h>
#include <QScroller>

class QAbstractScrollArea;

namespace KisKineticScroller {
KRITAWIDGETUTILS_EXPORT QScroller* createPreconfiguredScroller(QAbstractScrollArea *target);

KRITAWIDGETUTILS_EXPORT QScroller::ScrollerGestureType getConfiguredGestureType();

KRITAWIDGETUTILS_EXPORT void updateCursor(QWidget *source, QScroller::State state);
}

#endif
