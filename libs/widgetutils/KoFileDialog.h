/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2013-2014 Yue Liu <yue.liu@mail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOFILEDIALOG_H
#define KOFILEDIALOG_H

#include "kritawidgetutils_export.h"

#include <QFileDialog>
#include <QString>
#include <QStringList>

class KRITAWIDGETUTILS_EXPORT KoFileDialog : public QObject
{
    Q_OBJECT

public:
    enum DialogType {
        OpenFile,
        OpenFiles,
        OpenDirectory,
        ImportFile,
        ImportFiles,
        ImportDirectory,
        SaveFile
    };

    KoFileDialog(QWidget *parent,
                 KoFileDialog::DialogType type,
                 const QString &dialogName);

    ~KoFileDialog() override;

    void setCaption(const QString &caption);

    void setDefaultDir(const QString &defaultDir, bool force = false);

    void setDirectoryUrl(const QUrl &defaultUri);

    void setImageFilters();

    void setMimeTypeFilters(const QStringList &mimeTypeList,
                            QString defaultMimeType = QString());

    void setNameFilter(const QString &filter);

    void selectNameFilter(const QString &filter);

    QStringList filenames();

    QString filename();

    QString selectedNameFilter() const;

    QString selectedMimeType() const;

public Q_SLOTS:
    void onFilterSelected(const QString &filter);

private:
    static QStringList splitNameFilter(const QString &nameFilter, QStringList *mimeList);

    void createFileDialog();

    QString getUsedDir(const QString &dialogName);
    void saveUsedDir(const QString &fileName, const QString &dialogName);

    const QPair<QStringList, QMap<QString, QString>> getFilterStringListFromMime(const QStringList &mimeList,
                                                  bool withAllSupportedEntry = false);

    class Private;
    Private * const d;
};

#endif
