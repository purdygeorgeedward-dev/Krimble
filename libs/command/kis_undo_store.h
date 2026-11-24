/*
 *  SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_UNDO_STORE_H_
#define KIS_UNDO_STORE_H_

#include <QObject>

#include <kritacommand_export.h>

class KUndo2Command;
class KUndo2MagicString;

class KRITACOMMAND_EXPORT KisUndoStore : public QObject
{
    Q_OBJECT
public:
    KisUndoStore();
    virtual ~KisUndoStore();

public:

    virtual const KUndo2Command* presentCommand() = 0;
    virtual void undoLastCommand() = 0;
    virtual void addCommand(KUndo2Command *cmd) = 0;
    virtual void beginMacro(const KUndo2MagicString& macroName) = 0;
    virtual void endMacro() = 0;
    virtual void purgeRedoState() = 0;

Q_SIGNALS:
    void historyStateChanged();

private:
    Q_DISABLE_COPY(KisUndoStore)
};

#endif

