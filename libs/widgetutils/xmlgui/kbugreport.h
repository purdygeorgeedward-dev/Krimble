/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KBUGREPORT_H
#define KBUGREPORT_H

#include <QDialog>
#include <kritawidgetutils_export.h>

class KAboutData;
class KisKBugReportPrivate;

class KRITAWIDGETUTILS_EXPORT KisKBugReport : public QDialog
{
    Q_OBJECT

public:
    explicit KisKBugReport(const KAboutData &aboutData, QWidget *parent = 0L);

    ~KisKBugReport() override;

    void accept() override;

private:
    Q_PRIVATE_SLOT(d, void _k_updateUrl())

private:
    friend class KisKBugReportPrivate;
    KisKBugReportPrivate *const d;

    Q_DISABLE_COPY(KisKBugReport)
};

#endif

