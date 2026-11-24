/*
 * SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISTAGCHOOSERWIDGET_H
#define KISTAGCHOOSERWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include "kritaresourcewidgets_export.h"

#include <KisTag.h>
#include <KisTagModel.h>

class KRITARESOURCEWIDGETS_EXPORT KisTagChooserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KisTagChooserWidget(KisTagModel* model, QString resourceType, QWidget* parent);
    ~KisTagChooserWidget() override;

    void setCurrentItem(const QString &tag);

    int currentIndex() const;

    KisTagSP currentlySelectedTag();

    bool selectedTagIsReadOnly();

    void updateIcons();

Q_SIGNALS:

    void sigTagChosen(const KisTagSP tag);

public Q_SLOTS:

    void tagChanged(int index);

    void addTag(const QString &tag);
    void addTag(const QString &tag, KoResourceSP resource);
    void addTag(KisTagSP tag, KoResourceSP resource);

private Q_SLOTS:

    void tagToolRenameCurrentTag(const QString& tag);

    void tagToolDeleteCurrentTag();

    void tagToolUndeleteLastTag(KisTagSP tag);

    void tagToolContextMenuAboutToShow();

    void cacheSelectedTag();

    void restoreTagFromCache();

    void slotTagModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> roles);

private:

    void setCurrentIndex(int index);

    QMessageBox::ButtonRole overwriteTagDialog(KisTagChooserWidget* parent, bool undelete);

private:
    class Private;
    Private* const d;

};

#endif
