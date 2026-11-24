/*
    This file is part of the KDE project

    SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __koDocumentInfoDlg_h__
#define __koDocumentInfoDlg_h__

#include <kpagedialog.h>

class KoDocumentInfo;
class KPageWidgetItem;
class KoPageWidgetItem;

#include "kritaui_export.h"

class KRITAUI_EXPORT KoDocumentInfoDlg : public KPageDialog
{
    Q_OBJECT

public:
    KoDocumentInfoDlg(QWidget *parent, KoDocumentInfo* docInfo);

    ~KoDocumentInfoDlg() override;

    QList<KPageWidgetItem*> pages() const;

    bool isDocumentSaved();

    void setReadOnly(bool ro);

    void addPageItem(KoPageWidgetItem *item);

public Q_SLOTS:
    void accept() override;

protected:
    void hideEvent(QHideEvent * event) override;

private Q_SLOTS:
    void slotResetMetaData();

Q_SIGNALS:
    void saveRequested();

private:
    void initAboutTab();
    void initAuthorTab();
    void saveAboutData();

    void updateEditingTime();

    class KoDocumentInfoDlgPrivate;
    KoDocumentInfoDlgPrivate * const d;
};

#endif
