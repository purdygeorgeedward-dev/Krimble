/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_MACRO_BASED_UNDO_STORE_H
#define __KIS_MACRO_BASED_UNDO_STORE_H

#include <QScopedPointer>
#include "kis_undo_store.h"

class KisSavedMacroCommand;

class KisMacroBasedUndoStore : public KisUndoStore
{
public:
    KisMacroBasedUndoStore(KisSavedMacroCommand *command);
    ~KisMacroBasedUndoStore() override;

    const KUndo2Command* presentCommand() override;
    void undoLastCommand() override;
    void addCommand(KUndo2Command *cmd) override;
    void beginMacro(const KUndo2MagicString& macroName) override;
    void endMacro() override;
    void purgeRedoState() override;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
