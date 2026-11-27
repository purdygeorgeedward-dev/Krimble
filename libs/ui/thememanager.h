/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-02
 * Description : theme manager
 *
 * SPDX-FileCopyrightText: 2006-2011 Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QPixmap>
#include <QString>

#include <ksharedconfig.h>

class KisKActionCollection;
class KActionMenu;

namespace Digikam
{

class ThemeManager : public QObject
{
    Q_OBJECT

public:

    explicit ThemeManager(const QString &theme = "", QObject *parent = 0);
    ~ThemeManager() override;

    QString currentThemeName() const;
    void    setCurrentTheme(const QString& name);

    void    setThemeMenuAction(KActionMenu* const action);
    void    registerThemeActions(KisKActionCollection *actionCollection);

Q_SIGNALS:

    void signalThemeChanged();

private Q_SLOTS:

    void slotChangePalette();

private:
    void    populateThemeMap();
    void    populateThemeMenu();
    QPixmap createSchemePreviewIcon(const KSharedConfigPtr& config);

private:

    class ThemeManagerPriv;
    ThemeManagerPriv* const d;
};

}

#endif
