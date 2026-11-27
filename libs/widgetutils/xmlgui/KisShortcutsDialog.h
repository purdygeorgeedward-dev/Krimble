/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1997 Nicolas Hadacek <hadacek@kde.org>
    SPDX-FileCopyrightText: 2001 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2006 Hamish Rodda <rodda@kde.org>
    SPDX-FileCopyrightText: 2007 Roberto Raggi <roberto@kdevelop.org>
    SPDX-FileCopyrightText: 2007 Andreas Hartmetz <ahartmetz@gmail.com>
    SPDX-FileCopyrightText: 2008 Michael Jansen <kde@michael-jansen.biz>
    SPDX-FileCopyrightText: 2015 Michael Abrahams <miabraha@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KISSHORTCUTSDIALOG_H
#define KISSHORTCUTSDIALOG_H

#include <KisShortcutsEditor.h>

const auto defaultActionTypes = KisShortcutsEditor::WidgetAction \
                                | KisShortcutsEditor::WindowAction  \
                                | KisShortcutsEditor::ApplicationAction;

class KRITAWIDGETUTILS_EXPORT KisShortcutsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit KisShortcutsDialog(KisShortcutsEditor::ActionTypes types = defaultActionTypes,
                                KisShortcutsEditor::LetterShortcuts allowLetterShortcuts \
                                = KisShortcutsEditor::LetterShortcutsAllowed,
                                QWidget *parent = 0);

    ~KisShortcutsDialog() override;

    void addCollection(KisKActionCollection *, const QString &title = QString());

    QList<KisKActionCollection *> actionCollections() const;

    QSize sizeHint() const override;

    void save();
    void allDefault();
    void undo();

    void importConfiguration(const QString &path);

    void exportConfiguration(const QString &path) const;

    void loadCustomShortcuts(const QString &path);

    void saveCustomShortcuts(const QString &path) const;

private:
    Q_PRIVATE_SLOT(d, void changeShortcutScheme(const QString &))
    Q_PRIVATE_SLOT(d, void undo())

    class KisShortcutsDialogPrivate;
    class KisShortcutsDialogPrivate *const d;

    Q_DISABLE_COPY(KisShortcutsDialog)
};

#endif

