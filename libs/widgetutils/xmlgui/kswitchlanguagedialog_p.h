/*
 * This file is part of the KDE Libraries
 * SPDX-FileCopyrightText: 2007 Krzysztof Lichota (lichota@mimuw.edu.pl)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef _KSWITCHLANGUAGEDIALOG_H_
#define _KSWITCHLANGUAGEDIALOG_H_

#include <QDialog>

namespace KDEPrivate
{

class KisKSwitchLanguageDialogPrivate;

class KisKSwitchLanguageDialog : public QDialog
{
    Q_OBJECT

public:
    KisKSwitchLanguageDialog(QWidget *parent = nullptr);

    ~KisKSwitchLanguageDialog() override;

protected Q_SLOTS:
    virtual void slotOk();
    void slotDefault();

    virtual void languageOnButtonChanged(const QString &);

    virtual void slotAddLanguageButton();

    virtual void removeButtonClicked();

private:
    KisKSwitchLanguageDialogPrivate *const d;

    friend class KisKSwitchLanguageDialogPrivate;
};

}

#endif
