/*
 *    This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *    SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>
 *    SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 * */

#ifndef KISRESOURCEITEMCHOOSERCONTEXTMENU_H
#define KISRESOURCEITEMCHOOSERCONTEXTMENU_H

#include <QMenu>
#include <QWidgetAction>
#include <QLineEdit>

#include <KoResource.h>
#include <KisTag.h>
#include <KisTagModel.h>

#include "TagActions.h"
#include "KisTagChooserWidget.h"

class KisResourceItemChooserContextMenu :  public QMenu
{
    Q_OBJECT
public:
    explicit KisResourceItemChooserContextMenu(KoResourceSP resource, const KisTagSP currentlySelectedTag, KisTagChooserWidget *tagChooser);
    ~KisResourceItemChooserContextMenu() override;

Q_SIGNALS:

    void resourceTagAdditionRequested(const KisTagSP tag, KoResourceSP resource);

    void resourceTagRemovalRequested(KoResourceSP resource, const KisTagSP tag);

    void resourceAssignmentToNewTagRequested(const QString &tag, KoResourceSP resource);

public Q_SLOTS:

    void removeResourceExistingTag(const KisTagSP tag, KoResourceSP resource);

    void addResourceExistingTag(const KisTagSP tag, KoResourceSP resource);

private:
    KisTagModel *m_tagModel {0};
    KisTagChooserWidget *m_tagChooserWidget {0};

};

#endif
