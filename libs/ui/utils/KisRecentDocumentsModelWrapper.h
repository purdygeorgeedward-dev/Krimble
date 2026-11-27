/*
 *  SPDX-FileCopyrightText: 2021 Felipe Lema <felipelema@mortemale.org>
 *  SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISRECENTDOCUMENTSMODELWRAPPER_H
#define KISRECENTDOCUMENTSMODELWRAPPER_H

#include <QStandardItemModel>

class QString;
class QIcon;

class KisRecentDocumentsModelWrapper : public QObject
{
    Q_OBJECT
public:
    static constexpr const int ICON_SIZE_LENGTH = 200;

    static KisRecentDocumentsModelWrapper *instance();

private:
    KisRecentDocumentsModelWrapper();
    ~KisRecentDocumentsModelWrapper();

    void setFiles(const QList<QUrl> &urls);

public:
    QStandardItemModel &model();

private:
    QStandardItemModel m_filesAndThumbnailsModel;

private Q_SLOTS:
    void slotFileIconChanged(const QUrl &url, const QIcon &icon);
    void fileAdded(const QUrl &url);
    void fileRemoved(const QUrl &url);
    void listRenewed();

Q_SIGNALS:
    void sigModelIsUpToDate();
};

#endif
