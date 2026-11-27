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
    SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KRECENTFILESACTION_H
#define KRECENTFILESACTION_H

#include <kselectaction.h>
#include <qurl.h>
#include <kritawidgetutils_export.h>

class KConfigGroup;
class KRecentFilesActionPrivate;

class QIcon;
class QStandardItemModel;
class QStandardItem;

class KRITAWIDGETUTILS_EXPORT KRecentFilesAction : public KSelectAction
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(KRecentFilesAction)

public:
    explicit KRecentFilesAction(QObject *parent);

    KRecentFilesAction(const QString &text, QObject *parent);

    KRecentFilesAction(const QIcon &icon, const QString &text, QObject *parent);

    ~KRecentFilesAction() override;

    void addAction(QAction *action, const QUrl &url, const QString &name);

    QAction *removeAction(QAction *action) override;

private Q_SLOTS:
    virtual void clearActionTriggered();

public:
    void setRecentFilesModel(const QStandardItemModel *model);

Q_SIGNALS:
    void urlSelected(const QUrl &url);

private:
    void clearEntries();
    using KSelectAction::addAction;

    KRecentFilesActionPrivate *d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _k_urlSelected(QAction *))

    void rebuildEntries();

private Q_SLOTS:
    void fileAdded(const QUrl &url);
    void fileRemoved(const QUrl &url);
    void listRenewed();

    void modelItemChanged(QStandardItem *item);
    void modelRowsInserted(const QModelIndex &parent, int first, int last);

    void menuAboutToShow();
};

#endif
