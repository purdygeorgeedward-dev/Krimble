/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2014 Mohit Goyal <mohit.bits2011@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
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

#include <QDebug>
#include <klocalizedstring.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include "kundo2stack.h"
#include "kundo2stack_p.h"
#include "kundo2group.h"
#include <KoIcon.h>
#include <QtGlobal>
#include "kis_assert.h"

#ifndef QT_NO_UNDOCOMMAND

KUndo2Command::KUndo2Command(const KUndo2MagicString &text, KUndo2Command *parent)
    : m_hasParent(parent != 0)
    , m_endOfCommand(QTime::currentTime())
{
    d = new KUndo2CommandPrivate;
    if (parent != 0) {
        parent->d->child_list.append(this);
    }
    setText(text);
    setTime();
}

KUndo2Command::KUndo2Command(KUndo2Command *parent)
    : m_hasParent(parent != 0)
{
    d = new KUndo2CommandPrivate;
    if (parent != 0)
        parent->d->child_list.append(this);
    setTime();
}

KUndo2Command::~KUndo2Command()
{
    qDeleteAll(d->child_list);
    delete d;
}

int KUndo2Command::id() const
{
    return -1;
}

bool KUndo2Command::mergeWith(const KUndo2Command *command)
{
    Q_UNUSED(command);
    return false;
}

void KUndo2Command::redo()
{
    for (int i = 0; i < d->child_list.size(); ++i)
        d->child_list.at(i)->redo();
}

void KUndo2Command::undo()
{
    for (int i = d->child_list.size() - 1; i >= 0; --i)
        d->child_list.at(i)->undo();
}

QString KUndo2Command::actionText() const
{
    if (!d->actionText.isEmpty())
        return d->actionText;
    else
        return QString();
}

KUndo2MagicString KUndo2Command::text() const
{
    return d->text;
}

void KUndo2Command::setText(const KUndo2MagicString &undoText)
{
    d->text = undoText;
    d->actionText = undoText.toSecondaryString();
}

int KUndo2Command::childCount() const
{
    return d->child_list.count();
}

const KUndo2Command *KUndo2Command::child(int index) const
{
    if (index < 0 || index >= d->child_list.count())
        return 0;
    return d->child_list.at(index);
}

bool KUndo2Command::hasParent() const
{
    return m_hasParent;
}

int KUndo2Command::timedId() const
{
    return m_timedID;
}
void KUndo2Command::setTimedID(int value)
{
    m_timedID = value;
}

bool KUndo2Command::timedMergeWith(KUndo2Command *other)
{
    if(other->timedId() == this->timedId() && other->timedId() != -1 ) {
        m_mergeCommandsVector.append(other);
        m_timeOfCreation = other->time();
    } else {
        return false;
    }
    return true;
}
bool KUndo2Command::canAnnihilateWith(const KUndo2Command *other) const
{
    Q_UNUSED(other)
    return false;
}

void KUndo2Command::setTime()
{
    setTime(QTime::currentTime());
}

void KUndo2Command::setTime(const QTime &time)
{
    m_timeOfCreation = time;
}

QTime KUndo2Command::time() const
{
    return m_timeOfCreation;
}

void KUndo2Command::setEndTime()
{
    setEndTime(QTime::currentTime());
}
void KUndo2Command::setEndTime(const QTime &time)
{
    m_endOfCommand  = time;
}
QTime KUndo2Command::endTime() const
{
    return m_endOfCommand;
}

void KUndo2Command::undoMergedCommands()
{

    undo();
    if (!mergeCommandsVector().isEmpty()) {
        QVectorIterator<KUndo2Command*> it(mergeCommandsVector());
        it.toFront();
        while (it.hasNext()) {
            KUndo2Command* cmd = it.next();
            cmd->undoMergedCommands();
        }
    }
}

void KUndo2Command::redoMergedCommands()
{
    if (!mergeCommandsVector().isEmpty()) {

        QVectorIterator<KUndo2Command*> it(mergeCommandsVector());
        it.toBack();
        while (it.hasPrevious()) {
            KUndo2Command* cmd = it.previous();
            cmd->redoMergedCommands();
        }
    }
    redo();
}
QVector<KUndo2Command*> KUndo2Command::mergeCommandsVector() const
{
    return m_mergeCommandsVector;
}
bool KUndo2Command::isMerged() const
{
    return !m_mergeCommandsVector.isEmpty();
}

KUndo2CommandExtraData* KUndo2Command::extraData() const
{
    return d->extraData.data();
}

void KUndo2Command::setExtraData(KUndo2CommandExtraData *data)
{
    d->extraData.reset(data);
}

#endif

#ifndef QT_NO_UNDOSTACK

#ifndef QT_NO_ACTION

KUndo2Action::KUndo2Action(const QString &textTemplate, const QString &defaultText, QObject *parent)
    : QAction(parent)
    , m_textTemplate(textTemplate)
    , m_defaultText(defaultText)
{
}

void KUndo2Action::setPrefixedText(const QString &text)
{
    if (text.isEmpty())
        setText(m_defaultText);
    else
        setText(m_textTemplate.arg(text));
}

#endif

void KUndo2QStack::setIndex(int idx, bool clean)
{
    bool was_clean = m_index == m_clean_index;

    if (idx != m_index) {
        m_index = idx;
        Q_EMIT indexChanged(m_index);
        Q_EMIT canUndoChanged(canUndo());
        Q_EMIT undoTextChanged(undoText());
        Q_EMIT canRedoChanged(canRedo());
        Q_EMIT redoTextChanged(redoText());
    }

    if (clean)
        m_clean_index = m_index;

    bool is_clean = m_index == m_clean_index;
    if (is_clean != was_clean)
        Q_EMIT cleanChanged(is_clean);
}

void KUndo2QStack::purgeRedoState()
{
    bool macro = !m_macro_stack.isEmpty();
    if (macro) return;

    bool redoStateChanged = false;
    bool cleanStateChanged = false;

    while (m_index < m_command_list.size()) {
        delete m_command_list.takeLast();
        redoStateChanged = true;
    }

    if (m_clean_index > m_index) {
        m_clean_index = -1;
        cleanStateChanged = true;
    }

    if (redoStateChanged) {
        Q_EMIT canRedoChanged(canRedo());
        Q_EMIT redoTextChanged(redoText());
    }

    if (cleanStateChanged) {
        Q_EMIT cleanChanged(isClean());
    }
}

bool KUndo2QStack::checkUndoLimit()
{
    if (m_undo_limit <= 0 || !m_macro_stack.isEmpty() || m_undo_limit >= m_command_list.count())
        return false;

    int del_count = m_command_list.count() - m_undo_limit;

    for (int i = 0; i < del_count; ++i)
        delete m_command_list.takeFirst();

    m_index -= del_count;
    if (m_clean_index != -1) {
        if (m_clean_index < del_count)
            m_clean_index = -1;
        else
            m_clean_index -= del_count;
    }
    return true;
}

KUndo2QStack::KUndo2QStack(QObject *parent)
    : QObject(parent), m_index(0), m_clean_index(0), m_group(0), m_undo_limit(0)
    , m_useCumulativeUndoRedo(false)
{
#ifndef QT_NO_UNDOGROUP
    if (KUndo2Group *group = qobject_cast<KUndo2Group*>(parent))
        group->addStack(this);
#endif
}

KUndo2QStack::~KUndo2QStack()
{
#ifndef QT_NO_UNDOGROUP
    if (m_group != 0)
        m_group->removeStack(this);
#endif
    clear();
}

void KUndo2QStack::clear()
{
    if (m_command_list.isEmpty())
        return;

    bool was_clean = isClean();

    m_macro_stack.clear();
    qDeleteAll(m_command_list);
    m_command_list.clear();

    m_index = 0;
    m_clean_index = 0;

    Q_EMIT indexChanged(0);
    Q_EMIT canUndoChanged(false);
    Q_EMIT undoTextChanged(QString());
    Q_EMIT canRedoChanged(false);
    Q_EMIT redoTextChanged(QString());

    if (!was_clean)
        Q_EMIT cleanChanged(true);
}

void KUndo2QStack::push(KUndo2Command *cmd)
{
    cmd->redoMergedCommands();

    if (cmd->endTime().isNull()) {
        cmd->setEndTime();
    }

    bool macro = !m_macro_stack.isEmpty();

    KUndo2Command *cur = 0;
    if (macro) {
        KUndo2Command *macro_cmd = m_macro_stack.last();
        if (!macro_cmd->d->child_list.isEmpty())
            cur = macro_cmd->d->child_list.last();
    } else {
        if (m_index > 0)
            cur = m_command_list.at(m_index - 1);
        while (m_index < m_command_list.size())
            delete m_command_list.takeLast();
        if (m_clean_index > m_index)
            m_clean_index = -1;
    }

    bool try_merge = cur != 0
                     && cur->id() != -1
                     && cur->id() == cmd->id()
                     && (macro || m_index != m_clean_index);

    if (!macro && m_command_list.size() > 1 && cmd->timedId() != -1 && m_useCumulativeUndoRedo) {
        KUndo2Command* lastcmd = m_command_list.last();

        auto tryMergeBack =
            [this] (auto revIt, bool *isMerged) {
                auto dst = std::prev(revIt.base());
                auto src = std::prev(dst);

                if ((*dst)->timedId() != -1 &&
                    (*dst)->timedId() == (*src)->timedId() &&
                    (*src)->time().msecsTo((*dst)->endTime()) <= m_cumulativeUndoData.maxGroupDuration &&
                    (*src)->endTime().msecsTo((*dst)->time()) <= m_cumulativeUndoData.maxGroupSeparation &&
                    (*dst)->timedMergeWith(*src)) {

                    const int removedIndex = std::distance(m_command_list.begin(), src);

                    if (m_clean_index - 1 == removedIndex) {
                        m_clean_index = -1;
                    } else if (m_clean_index - 1 > removedIndex) {
                        m_clean_index--;
                    }
                    m_index--;

                    dst = m_command_list.erase(src);
                    *isMerged = true;
                } else {
                    *isMerged = false;
                }

                return std::make_reverse_iterator(std::next(dst));
        };

        int extraCheckDepth = 0;
        int newDepth = 1;
        QTime oldTimeBase = lastcmd->time();
        QTime newTimeBase = cmd->time();

        auto it = std::make_reverse_iterator(m_command_list.end());
        auto next = std::next(it);

        while (next != std::make_reverse_iterator(m_command_list.begin())) {

            if (newDepth < m_cumulativeUndoData.excludeFromMerge) {
                ++it;
                ++newDepth;
                next = std::next(it);
                continue;
            }

            int oldTime = (*it)->endTime().msecsTo(oldTimeBase);
            int newTime = (*it)->endTime().msecsTo(newTimeBase);

            if (oldTime > m_cumulativeUndoData.mergeTimeout &&
                extraCheckDepth++ >= m_cumulativeUndoData.excludeFromMerge) break;

            if (newTime >= m_cumulativeUndoData.mergeTimeout) {
                bool merged = true;

                KUndo2Command *debugOldCmd = *it;

                it = tryMergeBack(it, &merged);

                KUndo2Command *debugNewCmd = *it;

                KIS_SAFE_ASSERT_RECOVER_BREAK(it != std::make_reverse_iterator(m_command_list.begin()));
                KIS_SAFE_ASSERT_RECOVER_BREAK(debugOldCmd == debugNewCmd);

                if (!merged) {
                    ++it;
                    ++newDepth;
                }

            } else {
                ++it;
                ++newDepth;
            }

            next = std::next(it);
        }
    }

    if (try_merge && !macro && cur->canAnnihilateWith(cmd)) {
        delete cmd;
        if (!macro) {
            Q_ASSERT(m_clean_index != m_index);

            delete m_command_list.takeLast();
            m_index--;

            Q_EMIT indexChanged(m_index);
            Q_EMIT canUndoChanged(canUndo());
            Q_EMIT undoTextChanged(undoText());
            Q_EMIT canRedoChanged(canRedo());
            Q_EMIT redoTextChanged(redoText());
        } else {
            delete m_macro_stack.takeLast();
        }

    } else if (try_merge && cur->mergeWith(cmd)) {
        delete cmd;
        if (!macro) {
            Q_EMIT indexChanged(m_index);
            Q_EMIT canUndoChanged(canUndo());
            Q_EMIT undoTextChanged(undoText());
            Q_EMIT canRedoChanged(canRedo());
            Q_EMIT redoTextChanged(redoText());
        }
    } else {
        if (macro) {
            m_macro_stack.last()->d->child_list.append(cmd);
        } else {
            m_command_list.append(cmd);

            checkUndoLimit();
            setIndex(m_index + 1, false);
        }
    }
}

void KUndo2QStack::setClean()
{
    if (!m_macro_stack.isEmpty()) {
        qWarning("KUndo2QStack::setClean(): cannot set clean in the middle of a macro");
        return;
    }

    setIndex(m_index, true);
}

bool KUndo2QStack::isClean() const
{
    if (!m_macro_stack.isEmpty())
        return false;
    return m_clean_index == m_index;
}

int KUndo2QStack::cleanIndex() const
{
    return m_clean_index;
}

void KUndo2QStack::undo()
{
    if (m_index == 0)
        return;

    if (!m_macro_stack.isEmpty()) {
        qWarning("KUndo2QStack::undo(): cannot undo in the middle of a macro");
        return;
    }

    int idx = m_index - 1;
    m_command_list.at(idx)->undoMergedCommands();
    setIndex(idx, false);
}

void KUndo2QStack::redo()
{
    if (m_index == m_command_list.size())
        return;

    if (!m_macro_stack.isEmpty()) {
        qWarning("KUndo2QStack::redo(): cannot redo in the middle of a macro");
        return;
    }

    m_command_list.at(m_index)->redoMergedCommands();
    setIndex(m_index + 1, false);
}

int KUndo2QStack::count() const
{
    return m_command_list.size();
}

int KUndo2QStack::index() const
{
    return m_index;
}

void KUndo2QStack::setIndex(int idx)
{
    if (!m_macro_stack.isEmpty()) {
        qWarning("KUndo2QStack::setIndex(): cannot set index in the middle of a macro");
        return;
    }

    if (idx < 0)
        idx = 0;
    else if (idx > m_command_list.size())
        idx = m_command_list.size();

    int i = m_index;
    while (i < idx) {
        m_command_list.at(i++)->redoMergedCommands();
        notifySetIndexChangedOneCommand();
    }
    while (i > idx) {
        m_command_list.at(--i)->undoMergedCommands();
        notifySetIndexChangedOneCommand();
    }

    setIndex(idx, false);
}

void KUndo2QStack::notifySetIndexChangedOneCommand()
{
}

bool KUndo2QStack::canUndo() const
{
    if (!m_macro_stack.isEmpty())
        return false;
    return m_index > 0;
}

bool KUndo2QStack::canRedo() const
{
    if (!m_macro_stack.isEmpty())
        return false;
    return m_index < m_command_list.size();
}

QString KUndo2QStack::undoText() const
{
    if (!m_macro_stack.isEmpty()) {
        return QString();
    }
    if (m_index > 0 && m_command_list.count() >= m_index -1 && m_command_list.at(m_index - 1) != 0) {
        return m_command_list.at(m_index - 1)->actionText();
    }
    return QString();
}

QString KUndo2QStack::redoText() const
{
    if (!m_macro_stack.isEmpty())
        return QString();
    if (m_index < m_command_list.size())
        return m_command_list.at(m_index)->actionText();
    return QString();
}

#ifndef QT_NO_ACTION

QAction *KUndo2QStack::createUndoAction(QObject *parent) const
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

QAction *KUndo2QStack::createRedoAction(QObject *parent) const
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

void KUndo2QStack::beginMacro(const KUndo2MagicString &text)
{
    KUndo2Command *cmd = new KUndo2Command();
    cmd->setText(text);

    if (m_macro_stack.isEmpty()) {
        while (m_index < m_command_list.size())
            delete m_command_list.takeLast();
        if (m_clean_index > m_index)
            m_clean_index = -1;
        m_command_list.append(cmd);
    } else {
        m_macro_stack.last()->d->child_list.append(cmd);
    }
    m_macro_stack.append(cmd);

    if (m_macro_stack.count() == 1) {
        Q_EMIT canUndoChanged(false);
        Q_EMIT undoTextChanged(QString());
        Q_EMIT canRedoChanged(false);
        Q_EMIT redoTextChanged(QString());
    }
}

void KUndo2QStack::endMacro()
{
    if (m_macro_stack.isEmpty()) {
        qWarning("KUndo2QStack::endMacro(): no matching beginMacro()");
        return;
    }

    m_macro_stack.removeLast();

    if (m_macro_stack.isEmpty()) {
        checkUndoLimit();
        setIndex(m_index + 1, false);
    }
}

const KUndo2Command *KUndo2QStack::command(int index) const
{
    if (index < 0 || index >= m_command_list.count())
        return 0;
    return m_command_list.at(index);
}

QString KUndo2QStack::text(int idx) const
{
    if (idx < 0 || idx >= m_command_list.size())
        return QString();
    return m_command_list.at(idx)->text().toString();
}

void KUndo2QStack::setUndoLimit(int limit)
{
    if (!m_command_list.isEmpty()) {
        qWarning("KUndo2QStack::setUndoLimit(): an undo limit can only be set when the stack is empty");
        return;
    }

    if (limit == m_undo_limit)
        return;
    m_undo_limit = limit;
    checkUndoLimit();
}

int KUndo2QStack::undoLimit() const
{
    return m_undo_limit;
}

void KUndo2QStack::setActive(bool active)
{
#ifdef QT_NO_UNDOGROUP
    Q_UNUSED(active);
#else
    if (m_group != 0) {
        if (active)
            m_group->setActiveStack(this);
        else if (m_group->activeStack() == this)
            m_group->setActiveStack(0);
    }
#endif
}

bool KUndo2QStack::isActive() const
{
#ifdef QT_NO_UNDOGROUP
    return true;
#else
    return m_group == 0 || m_group->activeStack() == this;
#endif
}
void KUndo2QStack::setUseCumulativeUndoRedo(bool value)
{
    m_useCumulativeUndoRedo = value;
}

bool KUndo2QStack::useCumulativeUndoRedo() const
{
    return m_useCumulativeUndoRedo;
}

void KUndo2QStack::setCumulativeUndoData(const KisCumulativeUndoData &data)
{
    m_cumulativeUndoData = data;
}

KisCumulativeUndoData KUndo2QStack::cumulativeUndoData()
{
    return m_cumulativeUndoData;
}

QAction* KUndo2Stack::createRedoAction(KisKActionCollection* actionCollection, const QString& actionName)
{
    QAction* action = KUndo2QStack::createRedoAction(actionCollection);

    if (actionName.isEmpty()) {
        action->setObjectName(KStandardAction::name(KStandardAction::Redo));
    } else {
        action->setObjectName(actionName);
    }

    action->setIcon(koIcon("edit-redo"));
    action->setIconText(i18n("Redo"));
    action->setShortcuts(KStandardShortcut::redo());

    actionCollection->addAction(action->objectName(), action);

    return action;
}

QAction* KUndo2Stack::createUndoAction(KisKActionCollection* actionCollection, const QString& actionName)
{
    QAction* action = KUndo2QStack::createUndoAction(actionCollection);

    if (actionName.isEmpty()) {
        action->setObjectName(KStandardAction::name(KStandardAction::Undo));
    } else {
        action->setObjectName(actionName);
    }

    action->setIcon(koIcon("edit-undo"));
    action->setIconText(i18n("Undo"));
    action->setShortcuts(KStandardShortcut::undo());

    actionCollection->addAction(action->objectName(), action);

    return action;
}

KUndo2Stack::KUndo2Stack(QObject *parent):
    KUndo2QStack(parent)
{
}

#endif
