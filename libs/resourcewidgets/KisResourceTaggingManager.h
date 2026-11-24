/*
 *    This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *    SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *    SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *    SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *    SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>
 *    SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCETAGGINGMANAGER_H
#define KISRESOURCETAGGINGMANAGER_H

#include <QObject>
#include <KisQStringListFwd.h>
#include <KoResource.h>
#include <KisTag.h>
#include <KisTagModel.h>

#include <kis_debug.h>

class QWidget;
class QString;
class QPoint;

class KisTagFilterWidget;
class KisTagChooserWidget;
class KisTagFilterResourceProxyModel;

class KisResourceTaggingManager : public QObject
{
    Q_OBJECT

public:

    explicit KisResourceTaggingManager(QString resourceType, KisTagFilterResourceProxyModel *model, QWidget *parent);

    ~KisResourceTaggingManager() override;

    void showTaggingBar(bool show);
    void contextMenuRequested(KoResourceSP currentResource, QPoint pos);
    KisTagFilterWidget *tagFilterWidget();
    KisTagChooserWidget *tagChooserWidget();

private Q_SLOTS:
    void tagChooserIndexChanged(const KisTagSP tag);
    void tagSearchLineEditTextChanged(const QString &lineEditText);
    void slotFilterByTagChanged(const bool filterByTag);

private:

    class Private;
    Private* const d;
};

#endif
