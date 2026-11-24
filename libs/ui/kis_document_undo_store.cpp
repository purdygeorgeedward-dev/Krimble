/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_document_undo_store.h"

#include "KisDocument.h"
#include <kundo2stack.h>

KisDocumentUndoStore::KisDocumentUndoStore(KisDocument *doc)
    : m_doc(doc)
{
    KIS_SAFE_ASSERT_RECOVER_NOOP(doc->thread() == this->thread());

    connect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SIGNAL(historyStateChanged()), Qt::DirectConnection);
}

const KUndo2Command* KisDocumentUndoStore::presentCommand()
{
    return m_doc->undoStack()->command(m_doc->undoStack()->index() - 1);
}

void KisDocumentUndoStore::undoLastCommand()
{
    m_doc->undoStack()->undo();
}

void KisDocumentUndoStore::addCommand(KUndo2Command *command)
{
    if(!command) return;
    m_doc->undoStack()->push(command);
}

void KisDocumentUndoStore::beginMacro(const KUndo2MagicString& macroName)
{
    m_doc->undoStack()->beginMacro(macroName);
}

void KisDocumentUndoStore::endMacro()
{
    m_doc->undoStack()->endMacro();
}

void KisDocumentUndoStore::purgeRedoState()
{
    m_doc->undoStack()->purgeRedoState();
}
