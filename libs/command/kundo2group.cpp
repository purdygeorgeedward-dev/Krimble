/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "kundo2group.h"
#include "kundo2stack.h"
#include "kundo2stack_p.h"
#include <klocalizedstring.h>

#ifndef QT_NO_UNDOGROUP

KUndo2Group::KUndo2Group(QObject *parent)
    : QObject(parent), m_active(0)
{
}

KUndo2Group::~KUndo2Group()
{
    QList<KUndo2QStack *>::iterator it = m_stack_list.begin();
    QList<KUndo2QStack *>::iterator end = m_stack_list.end();
    while (it != end) {
        (*it)->m_group = 0;
        ++it;
    }
}

void KUndo2Group::addStack(KUndo2QStack *stack)
{
    if (m_stack_list.contains(stack))
        return;
    m_stack_list.append(stack);

    if (KUndo2Group *other = stack->m_group)
        other->removeStack(stack);
    stack->m_group = this;
}

void KUndo2Group::removeStack(KUndo2QStack *stack)
{
    if (m_stack_list.removeAll(stack) == 0)
        return;
    if (stack == m_active)
        setActiveStack(0);
    stack->m_group = 0;
}

QList<KUndo2QStack*> KUndo2Group::stacks() const
{
    return m_stack_list;
}

void KUndo2Group::setActiveStack(KUndo2QStack *stack)
{
    if (m_active == stack)
        return;

    if (m_active != 0) {
        disconnect(m_active, SIGNAL(canUndoChanged(bool)),
                   this, SIGNAL(canUndoChanged(bool)));
        disconnect(m_active, SIGNAL(undoTextChanged(QString)),
                   this, SIGNAL(undoTextChanged(QString)));
        disconnect(m_active, SIGNAL(canRedoChanged(bool)),
                   this, SIGNAL(canRedoChanged(bool)));
        disconnect(m_active, SIGNAL(redoTextChanged(QString)),
                   this, SIGNAL(redoTextChanged(QString)));
        disconnect(m_active, SIGNAL(indexChanged(int)),
                   this, SIGNAL(indexChanged(int)));
        disconnect(m_active, SIGNAL(cleanChanged(bool)),
                   this, SIGNAL(cleanChanged(bool)));
    }

    m_active = stack;

    if (m_active == 0) {
        Q_EMIT canUndoChanged(false);
        Q_EMIT undoTextChanged(QString());
        Q_EMIT canRedoChanged(false);
        Q_EMIT redoTextChanged(QString());
        Q_EMIT cleanChanged(true);
        Q_EMIT indexChanged(0);
    } else {
        connect(m_active, SIGNAL(canUndoChanged(bool)),
                this, SIGNAL(canUndoChanged(bool)));
        connect(m_active, SIGNAL(undoTextChanged(QString)),
                this, SIGNAL(undoTextChanged(QString)));
        connect(m_active, SIGNAL(canRedoChanged(bool)),
                this, SIGNAL(canRedoChanged(bool)));
        connect(m_active, SIGNAL(redoTextChanged(QString)),
                this, SIGNAL(redoTextChanged(QString)));
        connect(m_active, SIGNAL(indexChanged(int)),
                this, SIGNAL(indexChanged(int)));
        connect(m_active, SIGNAL(cleanChanged(bool)),
                this, SIGNAL(cleanChanged(bool)));
        Q_EMIT canUndoChanged(m_active->canUndo());
        Q_EMIT undoTextChanged(m_active->undoText());
        Q_EMIT canRedoChanged(m_active->canRedo());
        Q_EMIT redoTextChanged(m_active->redoText());
        Q_EMIT cleanChanged(m_active->isClean());
        Q_EMIT indexChanged(m_active->index());
    }

    Q_EMIT activeStackChanged(m_active);
}

KUndo2QStack *KUndo2Group::activeStack() const
{
    return m_active;
}

void KUndo2Group::undo()
{
    if (m_active != 0)
        m_active->undo();
}

void KUndo2Group::redo()
{
    if (m_active != 0)
        m_active->redo();
}

bool KUndo2Group::canUndo() const
{
    return m_active != 0 && m_active->canUndo();
}

bool KUndo2Group::canRedo() const
{
    return m_active != 0 && m_active->canRedo();
}

QString KUndo2Group::undoText() const
{
    return m_active == 0 ? QString() : m_active->undoText();
}

QString KUndo2Group::redoText() const
{
    return m_active == 0 ? QString() : m_active->redoText();
}

bool KUndo2Group::isClean() const
{
    return m_active == 0 || m_active->isClean();
}

#ifndef QT_NO_ACTION

QAction *KUndo2Group::createUndoAction(QObject *parent) const
{
    KUndo2Action *result = new KUndo2Action(i18n("Undo %1"), i18nc("Default text for undo action", "Undo"), parent);
    result->setEnabled(canUndo());
    result->setPrefixedText(undoText());
    connect(this, SIGNAL(canUndoChanged(bool)),
            result, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(undoTextChanged(QString)),
            result, SLOT(setPrefixedText(QString)));
    connect(result, SIGNAL(triggered()), this, SLOT(undo()));
    return result;
}

QAction *KUndo2Group::createRedoAction(QObject *parent) const
{
    KUndo2Action *result = new KUndo2Action(i18n("Redo %1"), i18nc("Default text for redo action", "Redo"), parent);
    result->setEnabled(canRedo());
    result->setPrefixedText(redoText());
    connect(this, SIGNAL(canRedoChanged(bool)),
            result, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoTextChanged(QString)),
            result, SLOT(setPrefixedText(QString)));
    connect(result, SIGNAL(triggered()), this, SLOT(redo()));
    return result;
}

#endif

#endif
