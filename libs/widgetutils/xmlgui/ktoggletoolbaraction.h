/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
    SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
    SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
    SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
    SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2003 Andras Mantia <amantia@kde.org>
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KTOGGLETOOLBARACTION_H
#define KTOGGLETOOLBARACTION_H

#include <ktoggleaction.h>
#include <kritawidgetutils_export.h>

class KisToolBar;

class KRITAWIDGETUTILS_EXPORT KToggleToolBarAction : public KToggleAction
{
    Q_OBJECT

public:
    KToggleToolBarAction(const char *toolBarName, const QString &text, QObject *parent);

    KToggleToolBarAction(KisToolBar *toolBar, const QString &text, QObject *parent);

    ~KToggleToolBarAction() override;

    KisToolBar *toolBar();

    bool eventFilter(QObject *watched, QEvent *event) override;

private Q_SLOTS:
    void slotToggled(bool checked) override;

private:
    class Private;
    Private *const d;
};

#endif
