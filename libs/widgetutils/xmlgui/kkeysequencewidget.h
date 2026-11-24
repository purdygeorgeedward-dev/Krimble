/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2001, 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2007 Andreas Hartmetz <ahartmetz@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KKEYSEQUENCEWIDGET_H
#define KKEYSEQUENCEWIDGET_H

#include <kritawidgetutils_export.h>

#include <QList>
#include <QObject>
#include <QWidget>

class KisKKeySequenceWidgetPrivate;
class QAction;
class KisKActionCollection;

class KRITAWIDGETUTILS_EXPORT KisKKeySequenceWidget: public QWidget
{
    Q_OBJECT

    Q_FLAGS(ShortcutTypes)

    Q_PROPERTY(
        bool multiKeyShortcutsAllowed
        READ multiKeyShortcutsAllowed
        WRITE setMultiKeyShortcutsAllowed)

    Q_PROPERTY(
        ShortcutTypes checkForConflictsAgainst
        READ checkForConflictsAgainst
        WRITE setCheckForConflictsAgainst)

    Q_PROPERTY(
        bool modifierlessAllowed
        READ isModifierlessAllowed
        WRITE setModifierlessAllowed)

public:
    enum Validation {
        Validate = 0,
        NoValidate = 1
    };

    explicit KisKKeySequenceWidget(QWidget *parent = 0);

    ~KisKKeySequenceWidget() override;

    enum ShortcutType {
        None           = 0x00,
        LocalShortcuts = 0x01,
        StandardShortcuts = 0x02,
        GlobalShortcuts = 0x04
    };
    Q_DECLARE_FLAGS(ShortcutTypes, ShortcutType)

    void setCheckForConflictsAgainst(ShortcutTypes types);

    ShortcutTypes checkForConflictsAgainst() const;

    void setMultiKeyShortcutsAllowed(bool);
    bool multiKeyShortcutsAllowed() const;

    void setModifierlessAllowed(bool allow);

    bool isModifierlessAllowed();

    void setClearButtonShown(bool show);

    bool isKeySequenceAvailable(const QKeySequence &seq) const;

    QKeySequence keySequence() const;

    void setCheckActionCollections(const QList<KisKActionCollection *> &actionCollections);

    void setComponentName(const QString &componentName);

Q_SIGNALS:

    void keySequenceChanged(const QKeySequence &seq);

    void stealShortcut(const QKeySequence &seq, QAction *action);

public Q_SLOTS:

    void captureKeySequence();

    void setKeySequence(const QKeySequence &seq, Validation val = NoValidate);

    void clearKeySequence();

    void applyStealShortcut();

private:
    Q_PRIVATE_SLOT(d, void doneRecording())

private:
    friend class KisKKeySequenceWidgetPrivate;
    KisKKeySequenceWidgetPrivate *const d;

    Q_DISABLE_COPY(KisKKeySequenceWidget)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisKKeySequenceWidget::ShortcutTypes)

#endif
