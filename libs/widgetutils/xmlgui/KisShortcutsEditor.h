/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1997 Nicolas Hadacek <hadacek@kde.org>
    SPDX-FileCopyrightText: 2001, 2001 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2006 Hamish Rodda <rodda@kde.org>
    SPDX-FileCopyrightText: 2007 Roberto Raggi <roberto@kdevelop.org>
    SPDX-FileCopyrightText: 2007 Andreas Hartmetz <ahartmetz@gmail.com>
    SPDX-FileCopyrightText: 2008 Michael Jansen <kde@michael-jansen.biz>
    SPDX-FileCopyrightText: 2015 Michael Abrahams <miabraha@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KISSHORTCUTSEDITOR_H
#define KISSHORTCUTSEDITOR_H

#include <kritawidgetutils_export.h>

#include <QWidget>
#include <KisKineticScroller.h>

class KisKActionCollection;
class KConfig;
class KConfigBase;
class KConfigGroup;
class KGlobalAccel;
class KisShortcutsEditorPrivate;

class KRITAWIDGETUTILS_EXPORT KisShortcutsEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(ActionTypes actionTypes READ actionTypes WRITE setActionTypes)

public:
    enum ActionType {
        WidgetAction      = Qt::WidgetShortcut      ,
        WindowAction      = Qt::WindowShortcut      ,
        ApplicationAction = Qt::ApplicationShortcut ,
        GlobalAction      = 4,
        AllActions        = 0xffffffff
    };
    Q_DECLARE_FLAGS(ActionTypes, ActionType)

    enum LetterShortcuts {
        LetterShortcutsDisallowed = 0,
        LetterShortcutsAllowed
    };

    explicit KisShortcutsEditor(QWidget *parent,
                                ActionTypes actionTypes = AllActions, LetterShortcuts allowLetterShortcuts = LetterShortcutsAllowed);

    ~KisShortcutsEditor() override;

    bool isModified() const;

    void clearCollections();

    void clearSearch();

    void addCollection(KisKActionCollection *, const QString &title = QString());

    void undo();

    void save();

    void commit();

    void clearConfiguration();

    void saveShortcuts(KConfigGroup *config = 0) const;

    void exportConfiguration(KConfigBase *config) const;

    void importConfiguration(KConfigBase *config, bool isScheme);

    void setActionTypes(ActionTypes actionTypes);
    ActionTypes actionTypes() const;

Q_SIGNALS:
    void keyChange();

public Q_SLOTS:
    void resizeColumns();

    void allDefault();

    void printShortcuts() const;

    void searchUpdated(QString s);

    void slotScrollerStateChanged(QScroller::State state){KisKineticScroller::updateCursor(this, state);}

private:
    Q_PRIVATE_SLOT(d, void capturedShortcut(QVariant, const QModelIndex &))

private:
    friend class KisShortcutsDialog;
    friend class KisShortcutsEditorPrivate;
    KisShortcutsEditorPrivate *const d;
    Q_DISABLE_COPY(KisShortcutsEditor)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisShortcutsEditor::ActionTypes)

#endif
