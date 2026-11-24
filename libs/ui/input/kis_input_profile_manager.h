/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISINPUTPROFILEMANAGER_H
#define KISINPUTPROFILEMANAGER_H

#include <QObject>

#include "kritaui_export.h"

#define PROFILE_VERSION 6

class KisAbstractInputAction;
class KisInputProfile;
class KisShortcutConfiguration;

class KRITAUI_EXPORT KisInputProfileManager : public QObject
{
    Q_OBJECT
public:
    KisInputProfileManager(QObject *parent = 0);
    ~KisInputProfileManager() override;
    Q_DISABLE_COPY(KisInputProfileManager)

    KisInputProfile *profile(const QString &name) const;
    QList<KisInputProfile *> profiles() const;
    QStringList profileNames() const;

    KisInputProfile *currentProfile() const;
    void setCurrentProfile(KisInputProfile *profile);

    KisInputProfile *addProfile(const QString &name);
    void removeProfile(const QString &name);
    bool renameProfile(const QString &oldName, const QString &newName);
    void duplicateProfile(const QString &name, const QString &newName);

    QList< KisAbstractInputAction * > actions();

    void loadProfiles();
    void saveProfiles();

    void saveProfile(KisInputProfile *profile, QString storagePath);

    QList<KisShortcutConfiguration *> getConflictingShortcuts(KisInputProfile *profile);

    void resetAll();

    static KisInputProfileManager *instance();

Q_SIGNALS:
    void profilesChanged();
    void currentProfileChanged();

private:
    class Private;
    Private *const d;
};

#endif
