/**
 *  SPDX-FileCopyrightText: 1997 Nicolas Hadacek <hadacek@kde.org>
 *  SPDX-FileCopyrightText: 1998 Matthias Ettrich <ettrich@kde.org>
 *  SPDX-FileCopyrightText: 2001 Ellis Whitehead <ellis@kde.org>
 *  SPDX-FileCopyrightText: 2006 Hamish Rodda <rodda@kde.org>
 *  SPDX-FileCopyrightText: 2007 Roberto Raggi <roberto@kdevelop.org>
 *  SPDX-FileCopyrightText: 2007 Andreas Hartmetz <ahartmetz@gmail.com>
 *  SPDX-FileCopyrightText: 2008 Michael Jansen <kde@michael-jansen.biz>
 *  SPDX-FileCopyrightText: 2015 Michael Abrahams <miabraha@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KISSHORTCUTSEDITOR_P
#define KISSHORTCUTSEDITOR_P

#include "KisShortcutsEditor.h"
#include <QTreeWidget>
#include <ktreewidgetsearchline.h>

#include "KisShortcutsDialog_p.h"

#include "ui_KisShortcutsDialog.h"

class KisShortcutsEditorItem;

class KisShortcutsEditorPrivate
{
public:

  enum hierarchyLevel {Root = 0,
                       Program,
                       Action};

    KisShortcutsEditorPrivate(KisShortcutsEditor *q);

    void initGUI(KisShortcutsEditor::ActionTypes actionTypes, KisShortcutsEditor::LetterShortcuts allowLetterShortcuts);
    void appendToView(uint nList, const QString &title = QString());
    QTreeWidgetItem *findOrMakeItem(QTreeWidgetItem *parent, const QString &name);

    void allDefault();

    void clearConfiguration();

    void changeKeyShortcut(KisShortcutsEditorItem *item, uint column, const QKeySequence &capture);

    void capturedShortcut(const QVariant &, const QModelIndex &);

    bool addAction(QAction *action, QTreeWidgetItem *hier[], hierarchyLevel level);

    void printShortcuts() const;

    void setActionTypes(KisShortcutsEditor::ActionTypes actionTypes);

public:

    QList<KisKActionCollection *> actionCollections;
    KisShortcutsEditor *q;

    Ui::KisShortcutsDialog ui;

    KisShortcutsEditor::ActionTypes actionTypes;
    KisShortcutsEditorDelegate *delegate;
};

class QTreeWidgetHack : public QTreeWidget
{
public:
    QTreeWidgetItem *itemFromIndex(const QModelIndex &index) const
    {
        return QTreeWidget::itemFromIndex(index);
    }
    QModelIndex indexFromItem(QTreeWidgetItem *item, int column) const
    {
        return QTreeWidget::indexFromItem(item, column);
    }
};

class KisShortcutsEditorItem : public QTreeWidgetItem
{
public:

    KisShortcutsEditorItem(QTreeWidgetItem *parent, QAction *action);

    ~KisShortcutsEditorItem() override;

    void undo();

    void commit();

    QVariant data(int column, int role = Qt::DisplayRole) const override;
    bool operator<(const QTreeWidgetItem &other) const override;

    QKeySequence keySequence(uint column) const;
    void setKeySequence(uint column, const QKeySequence &seq);

    bool isModified(uint column) const;
    bool isModified() const;

    void setNameBold(bool flag)
    {
        m_isNameBold = flag;
    }

private:
    friend class KisShortcutsEditorPrivate;

    void updateModified();

    QAction *m_action;

    bool m_isNameBold{false};

    QList<QKeySequence> *m_oldLocalShortcut{0};

    QString m_actionNameInTable;

    QString m_id;

    QCollator m_collator;
};

#endif
