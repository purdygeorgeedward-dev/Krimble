/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KISOPENPANE_H
#define KISOPENPANE_H

#include <QDialog>
#include <QWidget>
#include <QPixmap>
#include <QList>
#include <KisQStringListFwd.h>

class KisDetailsPane;
class KisDocument;
class KisOpenPanePrivate;
class KisTemplatesPane;
class QPixmap;
class QString;
class QTreeWidgetItem;
class QUrl;

class KisOpenPane : public QDialog
{
    Q_OBJECT

public:
    KisOpenPane(QWidget *parent, const QStringList& mimeFilter, const QString& templatesResourcePath = QString());
    ~KisOpenPane() override;

    QTreeWidgetItem* addPane(const QString &title, const QString &untranslatedName, const QString &iconName, QWidget *widget, int sortWeight);
    QTreeWidgetItem* addPane(const QString &title, const QString &untranslatedName, const QPixmap& icon, QWidget* widget, int sortWeight);

    void addCustomDocumentWidget(QWidget *widget, const QString& title, const QString &untranslatedName, const QString& icon = QString());

    bool selectItem(const QString &name);

Q_SIGNALS:
    void documentSelected(KisDocument*);

protected Q_SLOTS:
    void updateSelectedWidget();
    void itemClicked(QTreeWidgetItem* item);

    void saveSplitterSizes(KisDetailsPane* sender, const QList<int>& sizes);

private Q_SLOTS:
    void openFileDialog();
    void slotOpenTemplate(const QUrl &url);

Q_SIGNALS:
    void openExistingFile(const QUrl&);
    void openTemplate(const QUrl&);

    void alwaysUseChanged(KisTemplatesPane* sender, const QString& alwaysUse);

    void splitterResized(KisDetailsPane* sender, const QList<int>& sizes);
    void cancelButton();

protected:
    void initTemplates(const QString& templatesResourcePath);

    void dragEnterEvent(QDragEnterEvent * event) override;
    void dropEvent(QDropEvent * event) override;

private:
    QStringList m_mimeFilter;

    KisOpenPanePrivate * const d;
};

#endif
