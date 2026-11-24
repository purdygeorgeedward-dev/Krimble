/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_FILE_NAME_REQUESTER_H
#define KIS_FILE_NAME_REQUESTER_H

#include "kritawidgets_export.h"

#include <QWidget>
#include <QString>
#include <QUrl>
#include <KoFileDialog.h>
#include <QValidator>

namespace Ui {
    class WdgFileNameRequester;
}

class KRITAWIDGETS_EXPORT KisFileNameRequester : public QWidget
{
    Q_OBJECT

public:
    explicit KisFileNameRequester(QWidget *parent = 0);
    ~KisFileNameRequester() override;

    void setStartDir(const QString &path);
    void setConfigurationName(const QString &name);

    QString fileName() const;

    void setMode(KoFileDialog::DialogType mode);
    KoFileDialog::DialogType mode() const;
    void setReadOnlyText(bool readonly);

    void setMimeTypeFilters(const QStringList &filterList, QString defaultFilter = QString());

    void setValidator(QValidator *validator);

public Q_SLOTS:
    void slotSelectFile();
    void setFileName(const QString &path);

Q_SIGNALS:
    void textChanged(const QString &fileName);
    void fileSelected(const QString &fileName);

private:
    QScopedPointer<Ui::WdgFileNameRequester> m_ui;
    QString m_basePath;
    KoFileDialog::DialogType m_mode;
    QStringList m_mime_filter_list;
    QString m_mime_default_filter;
    QString m_name;
};

#endif
