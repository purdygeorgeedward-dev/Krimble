/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Scott Petrovic <scottpetrovic@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISWELCOMEPAGEWIDGET_H
#define KISWELCOMEPAGEWIDGET_H

#include "kritaui_export.h"
#include "KisViewManager.h"
#include <KisUpdaterBase.h>
#include <KisKineticScroller.h>

#include <QAction>
#include <QWidget>
#include "ui_KisWelcomePage.h"
#include <QStandardItemModel>
#include <QScopedPointer>
#include <QFont>

#include "config-updaters.h"
class RecentItemDelegate;
class KisMainWindow;

class ShowNewsAction : public QAction
{
  Q_OBJECT
public:
    using QAction::QAction;
private Q_SLOTS:
    void enableFromLink(QString unused_url);
};

class KRITAUI_EXPORT KisWelcomePageWidget : public QWidget, public Ui::KisWelcomePage
{
    Q_OBJECT

    public:
    explicit KisWelcomePageWidget(QWidget *parent);
    ~KisWelcomePageWidget() override;

    void setMainWindow(KisMainWindow* m_mainWindow);

public Q_SLOTS:
    void showDropAreaIndicator(bool show);

    void slotUpdateThemeColors();

#ifdef ENABLE_UPDATERS
    void slotSetUpdateStatus(KisUpdaterStatus updateStatus);
    void slotShowUpdaterErrorDetails();
#endif

private Q_SLOTS:
    void slotNewFileClicked();
    void slotOpenFileClicked();
    void slotPaste();

    void recentDocumentClicked(QModelIndex index);
    void slotRecentDocContextMenuRequest(const QPoint &pos);

    void slotRecentFilesModelIsUpToDate();

    void slotScrollerStateChanged(QScroller::State state){ KisKineticScroller::updateCursor(this, state); }

#ifdef ENABLE_UPDATERS
    void slotRunVersionUpdate();
    void slotToggleUpdateChecks(bool state);
#endif

#ifdef Q_OS_ANDROID
    void slotUpdateDonationState();
#endif

    bool isDevelopmentBuild();

    QFont largerFont();

protected:

    void dragEnterEvent(QDragEnterEvent * event) override;
    void dropEvent(QDropEvent * event) override;
    void dragMoveEvent(QDragMoveEvent * event) override;
    void dragLeaveEvent(QDragLeaveEvent * event) override;
    void changeEvent(QEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupNewsLangSelection(QMenu *newsOptionMenu);
    void showDevVersionHighlight();

    static void updateShortcutLink(QToolButton *button, QLabel *label, QAction *action);

#ifdef ENABLE_UPDATERS
    void updateVersionUpdaterFrame();
#endif

#ifdef Q_OS_ANDROID
    void initDonations();
#endif

    KisMainWindow *m_mainWindow {nullptr};

    const QString analyticsString = "pk_campaign=startup-sceen&pk_kwd=";

    QColor textColor;
    QColor backgroundColor;
    QColor blendedColor;
    QString blendedStyle;

#ifdef ENABLE_UPDATERS
    QScopedPointer<KisUpdaterBase> m_versionUpdater;
    KisUpdaterStatus m_updaterStatus;
#endif
    bool m_networkIsAllowed {false};

    QScopedPointer<RecentItemDelegate> recentItemDelegate;

};

#endif
