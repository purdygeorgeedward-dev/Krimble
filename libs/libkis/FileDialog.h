/*
 *  SPDX-FileCopyrightText: 2013-2014 Yue Liu <yue.liu@mail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LIBKIS_FILEDIALOG_H
#define LIBKIS_FILEDIALOG_H

#include "kritalibkis_export.h"

#include <QDialog>

class KRITALIBKIS_EXPORT FileDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(FileDialog)

public:
    enum DialogType
    {
        OpenFile,
        OpenFiles,
        OpenDirectory,
        ImportFile,
        ImportFiles,
        ImportDirectory,
        SaveFile
    };

    FileDialog(QWidget *parent = nullptr,
               const FileDialog::DialogType = DialogType::OpenFile,
               const QString &dialogName = "");

    ~FileDialog() override;

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

    static QString getOpenFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString(), const QString &selectedFilter = QString(), const QString &dialogName = QString());

    static QStringList getOpenFileNames(QWidget *parent = nullptr, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString(), const QString &selectedFilter = QString(), const QString &dialogName = QString());

    static QString getExistingDirectory(QWidget *parent = nullptr, const QString &caption = QString(), const QString &directory = QString(), const QString &dialogName = QString());

    static QString getSaveFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString(), const QString &selectedFilter = QString(), const QString &dialogName = QString());

    QString selectedNameFilter() const;

    QString selectedMimeType() const;

public Q_SLOTS:
    void onFilterSelected(const QString &filter);

private:

    struct Private;
    QScopedPointer<Private> const d;
};

#endif
