/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KUNDOACTIONS_H
#define KUNDOACTIONS_H

#include <kritawidgetutils_export.h>

#include <QString>

class KisKActionCollection;
class QAction;
class QUndoStack;

namespace KisKUndoActions
{
KRITAWIDGETUTILS_EXPORT QAction *createRedoAction(QUndoStack *undoStack, KisKActionCollection *actionCollection, const QString &actionName = QString());

KRITAWIDGETUTILS_EXPORT QAction *createUndoAction(QUndoStack *undoStack, KisKActionCollection *actionCollection, const QString &actionName = QString());
}

#endif
