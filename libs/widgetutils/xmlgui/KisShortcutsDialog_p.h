/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2006, 2007 Andreas Hartmetz (ahartmetz@gmail.com)
    SPDX-FileCopyrightText: 2008 Michael Jansen <kde@michael-jansen.biz>
    SPDX-FileCopyrightText: 2008 Alexander Dymo <adymo@kdevelop.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KISSHORTCUTSDIALOG_P_H
#define KISSHORTCUTSDIALOG_P_H

#include "KisShortcutsEditor.h"
#include "kkeysequencewidget.h"
#include "KisShortcutsDialog.h"

#include <kextendableitemdelegate.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>

#include <QKeySequence>
#include <QMetaType>
#include <QModelIndex>
#include <QList>
#include <QCollator>
#include <QHBoxLayout>

class QLabel;
class QTreeWidget;
class QTreeWidgetItem;
class QRadioButton;
class QAction;
class KisKActionCollection;
class QPushButton;
class QComboBox;
class KisShortcutsDialog;
class KisKShortcutSchemesEditor;
class QAction;

enum ColumnDesignation {
    Name = 0,
    LocalPrimary,
    LocalAlternate,
    Id
};

enum MyRoles {
    ShortcutRole = Qt::UserRole,
    DefaultShortcutRole,
    ObjectRole
};

enum ItemTypes {
    NonActionItem = 0,
    ActionItem = 1
};

QKeySequence primarySequence(const QList<QKeySequence> &sequences);

QKeySequence alternateSequence(const QList<QKeySequence> &sequences);

class KisShortcutsDialog::KisShortcutsDialogPrivate
{
public:
    KisShortcutsDialogPrivate(KisShortcutsDialog *q);
    void changeShortcutScheme(const QString &scheme);
    void undo();
    void save();

    QHash<QString, KisKActionCollection *> m_collections;
    KisShortcutsDialog *q;
    KisShortcutsEditor *m_shortcutsEditor {0};
    KisKShortcutSchemesEditor *m_schemeEditor{0};
};

class KisShortcutsEditorDelegate : public KExtendableItemDelegate
{
    Q_OBJECT
public:
    KisShortcutsEditorDelegate(QTreeWidget *parent, bool allowLetterShortcuts);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setCheckActionCollections(const QList<KisKActionCollection *> checkActionCollections);
    bool eventFilter(QObject *, QEvent *) override;
private:
    mutable QPersistentModelIndex m_editingIndex;
    bool m_allowLetterShortcuts;
    QWidget *m_editor;

    QList<KisKActionCollection *> m_checkActionCollections;

Q_SIGNALS:
  void shortcutChanged(QVariant, const QModelIndex &);

public Q_SLOTS:
  void hiddenBySearchLine(QTreeWidgetItem *, bool);

private Q_SLOTS:
    void itemActivated(QModelIndex index);

    void itemCollapsed(QModelIndex index);

    void stealShortcut(const QKeySequence &seq, QAction *action);

    void keySequenceChanged(const QKeySequence &);

};

class ShortcutEditWidget : public QWidget
{
    Q_OBJECT
public:
    ShortcutEditWidget(QWidget *viewport, const QKeySequence &defaultSeq, const QKeySequence &activeSeq,
                       bool allowLetterShortcuts);

    void setCheckActionCollections(const QList<KisKActionCollection *> checkActionCollections);

    KisKKeySequenceWidget::ShortcutTypes checkForConflictsAgainst() const;
    void setCheckForConflictsAgainst(KisKKeySequenceWidget::ShortcutTypes);

    bool multiKeyShortcutsAllowed() const;
    void setMultiKeyShortcutsAllowed(bool);

    void setComponentName(const QString componentName);

    void setAction(QObject *action);
    void paintEvent(QPaintEvent *pe) override;

Q_SIGNALS:
    void keySequenceChanged(const QKeySequence &);

    void stealShortcut(const QKeySequence &seq, QAction *action);

public Q_SLOTS:
  void setKeySequence(const QKeySequence &activeSeq);

private Q_SLOTS:
    void defaultToggled(bool);
    void setCustom(const QKeySequence &);

private:
    QLabel *m_defaultLabel;
    QKeySequence m_defaultKeySequence;
    QRadioButton *m_defaultRadio;
    QRadioButton *m_customRadio;
    KisKKeySequenceWidget *m_customEditor;
    bool m_isUpdating;
    QObject *m_action;
};

#endif
