/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2007 Urs Wolfer <uwolfer at kde.org>

   Parts of this class have been take from the KAboutKDE class, which was
   SPDX-FileCopyrightText: 2000 Espen Sand <espen@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KABOUT_KDE_DIALOG_H
#define KABOUT_KDE_DIALOG_H

#include <QDialog>

namespace KDEPrivate
{

class KisKAboutKdeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KisKAboutKdeDialog(QWidget *parent = 0);

private:
    class Private;
    Private *const d;
    Q_DISABLE_COPY(KisKAboutKdeDialog)
};

}

#endif
