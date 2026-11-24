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

#include "KisShortcutsEditor.h"
#include "KisShortcutsEditor_p.h"
#include <QHeaderView>
#include <QTreeWidget>
#include <QDebug>
#include <QTextTable>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <ksharedconfig.h>
#include <KConfigGroup>
#include "kis_action_registry.h"
#include <KisKineticScroller.h>

namespace {
    KisShortcutsEditorItem *itemFromIndex(QTreeWidget *const w, const QModelIndex &index)
    {
        QTreeWidgetItem *item = static_cast<QTreeWidgetHack *>(w)->itemFromIndex(index);
        if (item && item->type() == ActionItem) {
            return static_cast<KisShortcutsEditorItem *>(item);
        }
        return 0;
    }
}

KisShortcutsEditorPrivate::KisShortcutsEditorPrivate(KisShortcutsEditor *q)
    :   q(q),
        delegate(0)
{}

void KisShortcutsEditorPrivate::initGUI(KisShortcutsEditor::ActionTypes types,
                                        KisShortcutsEditor::LetterShortcuts allowLetterShortcuts)
{
    actionTypes = types;

    ui.setupUi(q);
    q->layout()->setContentsMargins(0, 0, 0, 0);
    ui.searchFilter->searchLine()->setTreeWidget(ui.list);
    ui.list->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    delegate = new KisShortcutsEditorDelegate(
        ui.list,
        allowLetterShortcuts == KisShortcutsEditor::LetterShortcutsAllowed);

    ui.list->setItemDelegate(delegate);
    ui.list->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui.list->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.list->setAlternatingRowColors(true);

    QScroller *scroller = KisKineticScroller::createPreconfiguredScroller(ui.list);
    if (scroller){
        QObject::connect(scroller, SIGNAL(stateChanged(QScroller::State)),
                         q, SLOT(slotScrollerStateChanged(QScroller::State)));
    }

    QObject::connect(delegate, SIGNAL(shortcutChanged(QVariant,QModelIndex)),
                     q, SLOT(capturedShortcut(QVariant,QModelIndex)));
    QObject::connect(ui.searchFilter->searchLine(), SIGNAL(hiddenChanged(QTreeWidgetItem*,bool)),
                     delegate, SLOT(hiddenBySearchLine(QTreeWidgetItem*,bool)));
    QObject::connect(ui.searchFilter->searchLine(), SIGNAL(searchUpdated(QString)),
                     q, SLOT(searchUpdated(QString)));

    ui.searchFilter->setFocus();
}

void KisShortcutsEditorPrivate::setActionTypes(KisShortcutsEditor::ActionTypes types)
{
    if (actionTypes == types) {
        return;
    }
    actionTypes = types;

    QHeaderView *header = ui.list->header();
    header->showSection(LocalPrimary);
    header->showSection(LocalAlternate);
}

bool KisShortcutsEditorPrivate::addAction(QAction *action, QTreeWidgetItem *hier[], hierarchyLevel level)
{

    QString actionName = action->objectName();
    if (actionName.isEmpty() || actionName.startsWith(QStringLiteral("unnamed-"))) {
        qCritical() << "Skipping action without name " << action->text() << "," << actionName << "!";
        return false;
    }

    const QVariant value = action->property("isShortcutConfigurable");
    if (!value.isValid() || value.toBool()) {
        new KisShortcutsEditorItem((hier[level]), action);
        return true;
    }

    return false;
}

void KisShortcutsEditorPrivate::allDefault()
{
    for (QTreeWidgetItemIterator it(ui.list); (*it); ++it) {
        if (!(*it)->parent() || (*it)->type() != ActionItem) {
            continue;
        }

        KisShortcutsEditorItem *item = static_cast<KisShortcutsEditorItem *>(*it);
        QAction *act = item->m_action;

        QList<QKeySequence> defaultShortcuts = act->property("defaultShortcuts").value<QList<QKeySequence> >();
        if (act->shortcuts() != defaultShortcuts) {
            QKeySequence primary = defaultShortcuts.isEmpty() ? QKeySequence() : defaultShortcuts.at(0);
            QKeySequence alternate = defaultShortcuts.size() <= 1 ? QKeySequence() : defaultShortcuts.at(1);
            changeKeyShortcut(item, LocalPrimary, primary);
            changeKeyShortcut(item, LocalAlternate, alternate);
        }
    }
}

QTreeWidgetItem *KisShortcutsEditorPrivate::findOrMakeItem(QTreeWidgetItem *parent, const QString &name)
{
    for (int i = 0; i < parent->childCount(); i++) {
        QTreeWidgetItem *child = parent->child(i);
        if (child->text(0) == name) {
            return child;
        }
    }
    QTreeWidgetItem *ret = new QTreeWidgetItem(parent, NonActionItem);
    ret->setText(0, name);
    ui.list->expandItem(ret);
    ret->setFlags(ret->flags() & ~Qt::ItemIsSelectable);
    return ret;
}

void KisShortcutsEditorPrivate::capturedShortcut(const QVariant &newShortcut, const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    int column = index.column();
    KisShortcutsEditorItem *item = itemFromIndex(ui.list, index);
    Q_ASSERT(item);

    if (column >= LocalPrimary && column <= Id) {
        changeKeyShortcut(item, column, newShortcut.value<QKeySequence>());
    }
}

void KisShortcutsEditorPrivate::changeKeyShortcut(KisShortcutsEditorItem *item, uint column, const QKeySequence &capture)
{
    if (capture == item->keySequence(column)) {
        return;
    }

    item->setKeySequence(column, capture);
    q->keyChange();
    item->setText(column, capture.toString(QKeySequence::NativeText));
}

void KisShortcutsEditorPrivate::clearConfiguration()
{
    for (QTreeWidgetItemIterator it(ui.list); (*it); ++it) {
        if (!(*it)->parent()) {
            continue;
        }

        KisShortcutsEditorItem *item = static_cast<KisShortcutsEditorItem *>(*it);

        changeKeyShortcut(item, LocalPrimary,   QKeySequence());
        changeKeyShortcut(item, LocalAlternate, QKeySequence());

    }
}

void KisShortcutsEditorPrivate::printShortcuts() const
{
    QTreeWidgetItem *root = ui.list->invisibleRootItem();
    QTextDocument doc;

    doc.setDefaultFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));

    QTextCursor cursor(&doc);
    cursor.beginEditBlock();
    QTextCharFormat headerFormat;
    headerFormat.setProperty(QTextFormat::FontSizeAdjustment, 3);
    headerFormat.setFontWeight(QFont::Bold);
    cursor.insertText(i18nc("header for an applications shortcut list", "Shortcuts for %1",
                            QGuiApplication::applicationDisplayName()),
                      headerFormat);
    QTextCharFormat componentFormat;
    componentFormat.setProperty(QTextFormat::FontSizeAdjustment, 2);
    componentFormat.setFontWeight(QFont::Bold);
    QTextBlockFormat componentBlockFormat = cursor.blockFormat();
    componentBlockFormat.setTopMargin(16);
    componentBlockFormat.setBottomMargin(16);

    QTextTableFormat tableformat;
    tableformat.setHeaderRowCount(1);
    tableformat.setCellPadding(4.0);
    tableformat.setCellSpacing(0);
    tableformat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableformat.setBorder(0.5);

    QList<QPair<QString, ColumnDesignation> > shortcutTitleToColumn;
    shortcutTitleToColumn << qMakePair(i18n("Main:"), LocalPrimary);
    shortcutTitleToColumn << qMakePair(i18n("Alternate:"), LocalAlternate);

    for (int i = 0; i < root->childCount(); i++) {
        QTreeWidgetItem *item = root->child(i);
        cursor.insertBlock(componentBlockFormat, componentFormat);
        cursor.insertText(item->text(0));

        QTextTable *table = cursor.insertTable(1, 3);
        table->setFormat(tableformat);
        int currow = 0;

        QTextTableCell cell = table->cellAt(currow, 0);
        QTextCharFormat format = cell.format();
        format.setFontWeight(QFont::Bold);
        cell.setFormat(format);
        cell.firstCursorPosition().insertText(i18n("Action Name"));

        cell = table->cellAt(currow, 1);
        cell.setFormat(format);
        cell.firstCursorPosition().insertText(i18n("Shortcuts"));

        cell = table->cellAt(currow, 2);
        cell.setFormat(format);
        cell.firstCursorPosition().insertText(i18n("Description"));
        currow++;

        for (QTreeWidgetItemIterator it(item); *it; ++it) {
            if ((*it)->type() != ActionItem) {
                continue;
            }

            KisShortcutsEditorItem *editoritem = static_cast<KisShortcutsEditorItem *>(*it);
            table->insertRows(table->rows(), 1);
            QVariant data = editoritem->data(Name, Qt::DisplayRole);
            table->cellAt(currow, 0).firstCursorPosition().insertText(data.toString());

            QTextTable *shortcutTable = 0;
            for (int k = 0; k < shortcutTitleToColumn.count(); k++) {
                data = editoritem->data(shortcutTitleToColumn.at(k).second, Qt::DisplayRole);
                QString key = data.value<QKeySequence>().toString();

                if (!key.isEmpty()) {
                    if (!shortcutTable) {
                        shortcutTable = table->cellAt(currow, 1).firstCursorPosition().insertTable(1, 2);
                        QTextTableFormat shortcutTableFormat = tableformat;
                        shortcutTableFormat.setCellSpacing(0.0);
                        shortcutTableFormat.setHeaderRowCount(0);
                        shortcutTableFormat.setBorder(0.0);
                        shortcutTable->setFormat(shortcutTableFormat);
                    } else {
                        shortcutTable->insertRows(shortcutTable->rows(), 1);
                    }
                    shortcutTable->cellAt(shortcutTable->rows() - 1, 0)\
                        .firstCursorPosition().insertText(shortcutTitleToColumn.at(k).first);
                    shortcutTable->cellAt(shortcutTable->rows() - 1, 1)\
                        .firstCursorPosition().insertText(key);
                }
            }

            QAction *action = editoritem->m_action;
            cell = table->cellAt(currow, 2);
            format = cell.format();
            format.setProperty(QTextFormat::FontSizeAdjustment, -1);
            cell.setFormat(format);
            cell.firstCursorPosition().insertHtml(action->whatsThis());

            currow++;
        }
        cursor.movePosition(QTextCursor::End);
    }
    cursor.endEditBlock();

    QPrinter printer;
    QPrintDialog *dlg = new QPrintDialog(&printer, q);
    if (dlg->exec() == QDialog::Accepted) {
        doc.print(&printer);
    }
    delete dlg;
}
