/*
 * This file is part of the KDE Libraries
 * SPDX-FileCopyrightText: 1999-2000 Espen Sand (espen@kde.org)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef KHELPMENU_H
#define KHELPMENU_H

#include <kritawidgetutils_export.h>

#include <QObject>
#include <QString>

class QMenu;
class QWidget;
class QAction;

class KAboutData;
class KisKHelpMenuPrivate;

class KRITAWIDGETUTILS_EXPORT KisKHelpMenu : public QObject
{
    Q_OBJECT

public:
    explicit KisKHelpMenu(QWidget *parent = 0, const QString &aboutAppText = QString(),
                       bool showWhatsThis = true);

    KisKHelpMenu(QWidget *parent, const KAboutData &aboutData,
              bool showWhatsThis = true);

    ~KisKHelpMenu() override;

    QMenu *menu();

    enum MenuId {
        menuHelpContents = 0,
        menuWhatsThis = 1,
        menuAboutApp = 2,
        menuAboutKDE = 3,
        menuReportBug = 4,
        menuSwitchLanguage = 5
    };

    QAction *action(MenuId id) const;

public Q_SLOTS:
    void appHelpActivated();

    void contextHelpActivated();

    void aboutApplication();

    void aboutKDE();

    void reportBug();

    void switchApplicationLanguage();

private Q_SLOTS:
    void menuDestroyed();

    void dialogFinished();

    void timerExpired();

Q_SIGNALS:
    void showAboutApplication();

private:
    KisKHelpMenuPrivate *const d;
};

#endif
