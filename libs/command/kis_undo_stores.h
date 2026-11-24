/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_UNDO_STORES_H
#define __KIS_UNDO_STORES_H

#include "kis_undo_store.h"

class KUndo2Stack;
class KUndo2MagicString;

class KRITACOMMAND_EXPORT KisSurrogateUndoStore : public KisUndoStore
{
public:
    KisSurrogateUndoStore();
    ~KisSurrogateUndoStore() override;

    const KUndo2Command* presentCommand() override;
    void undoLastCommand() override;
    void addCommand(KUndo2Command *cmd) override;
    void beginMacro(const KUndo2MagicString& macroName) override;
    void endMacro() override;

    void undo();
    void redo();

    void undoAll();
    void redoAll();

    void purgeRedoState() override;

    void clear();

private:
    KUndo2Stack *m_undoStack;
};

class KRITACOMMAND_EXPORT KisDumbUndoStore : public KisUndoStore
{
public:
    const KUndo2Command* presentCommand() override;
    void undoLastCommand() override;
    void addCommand(KUndo2Command *cmd) override;
    void beginMacro(const KUndo2MagicString& macroName) override;
    void endMacro() override;
    void purgeRedoState() override;
};

#endif
