/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_input_profile_manager.h"
#include "kis_input_profile.h"

#include <QMap>
#include <QStringList>
#include <QDir>
#include <QGlobalStatic>
#include <QRegularExpression>

#include <KoResourcePaths.h>
#include <kconfig.h>
#include <kconfiggroup.h>

#include "kis_config.h"
#include "kis_alternate_invocation_action.h"
#include "kis_change_primary_setting_action.h"
#include "kis_pan_action.h"
#include "kis_rotate_canvas_action.h"
#include "KisPopupWidgetAction.h"
#include "kis_tool_invocation_action.h"
#include "kis_zoom_action.h"
#include "kis_shortcut_configuration.h"
#include "kis_select_layer_action.h"
#include "kis_gamma_exposure_action.h"
#include "kis_change_frame_action.h"
#include "kis_zoom_and_rotate_action.h"
#include "KisCanvasOnlyAction.h"
#include "KisTouchGestureAction.h"
#include "KisInputProfileMigrator.h"

class Q_DECL_HIDDEN KisInputProfileManager::Private
{
public:
    Private() : currentProfile(0) { }

    void createActions();
    QString profileFileName(const QString &profileName);

    KisInputProfile *currentProfile;

    QMap<QString, KisInputProfile *> profiles;

    QList<KisAbstractInputAction *> actions;
};

Q_GLOBAL_STATIC(KisInputProfileManager, inputProfileManager)

KisInputProfileManager *KisInputProfileManager::instance()
{
    return inputProfileManager;
}

QList< KisInputProfile * > KisInputProfileManager::profiles() const
{
    return d->profiles.values();
}

QStringList KisInputProfileManager::profileNames() const
{
    return d->profiles.keys();
}

KisInputProfile *KisInputProfileManager::profile(const QString &name) const
{
    if (d->profiles.contains(name)) {
        return d->profiles.value(name);
    }

    return 0;
}

KisInputProfile *KisInputProfileManager::currentProfile() const
{
    return d->currentProfile;
}

void KisInputProfileManager::setCurrentProfile(KisInputProfile *profile)
{
    if (profile && profile != d->currentProfile) {
        d->currentProfile = profile;
        Q_EMIT currentProfileChanged();
    }
}

KisInputProfile *KisInputProfileManager::addProfile(const QString &name)
{
    if (d->profiles.contains(name)) {
        return d->profiles.value(name);
    }

    KisInputProfile *profile = new KisInputProfile(this);
    profile->setName(name);
    d->profiles.insert(name, profile);

    Q_EMIT profilesChanged();

    return profile;
}

void KisInputProfileManager::removeProfile(const QString &name)
{
    if (d->profiles.contains(name)) {
        QString currentProfileName = d->currentProfile->name();

        delete d->profiles.value(name);
        d->profiles.remove(name);

        QDir userDir(KoResourcePaths::saveLocation("data", "input/"));

        if (userDir.exists(d->profileFileName(name))) {
            userDir.remove(d->profileFileName(name));
        }

        if (currentProfileName == name) {
            d->currentProfile = d->profiles.begin().value();
            Q_EMIT currentProfileChanged();
        }

        Q_EMIT profilesChanged();
    }
}

bool KisInputProfileManager::renameProfile(const QString &oldName, const QString &newName)
{
    if (!d->profiles.contains(oldName)) {
        return false;
    }

    KisInputProfile *profile = d->profiles.value(oldName);
    if (profile) {
        d->profiles.remove(oldName);
        profile->setName(newName);
        d->profiles.insert(newName, profile);

        Q_EMIT profilesChanged();

        return true;
    }

    return false;
}

void KisInputProfileManager::duplicateProfile(const QString &name, const QString &newName)
{
    if (!d->profiles.contains(name) || d->profiles.contains(newName)) {
        return;
    }

    KisInputProfile *newProfile = new KisInputProfile(this);
    newProfile->setName(newName);
    d->profiles.insert(newName, newProfile);

    KisInputProfile *profile = d->profiles.value(name);
    QList<KisShortcutConfiguration *> shortcuts = profile->allShortcuts();
    Q_FOREACH(KisShortcutConfiguration * shortcut, shortcuts) {
        newProfile->addShortcut(new KisShortcutConfiguration(*shortcut));
    }

    Q_EMIT profilesChanged();
}

QList< KisAbstractInputAction * > KisInputProfileManager::actions()
{
    return d->actions;
}

void KisInputProfileManager::loadProfiles()
{
    d->currentProfile = nullptr;
    qDeleteAll(d->profiles);
    d->profiles.clear();

    QStringList profiles = KoResourcePaths::findAllAssets("data", "input/*.profile", KoResourcePaths::Recursive);

    dbgKrita << "profiles" << profiles;

    QMap<QString, ProfileEntry> profileEntriesToMigrate;
    QMap<QString, QList<ProfileEntry>> profileEntries;

    KisConfig cfg(true);

    Q_FOREACH(const QString & p, profiles) {

        ProfileEntry entry;
        entry.fullpath = p;

        KConfig config(p, KConfig::SimpleConfig);
        if (!config.hasGroup("General") || !config.group("General").hasKey("name") || !config.group("General").hasKey("version")) {
            continue;
        }

        entry.version = config.group("General").readEntry("version", 0);
        entry.name = config.group("General").readEntry("name");

        if (entry.version == PROFILE_VERSION - 1) {
            profileEntriesToMigrate[entry.name] = entry;

        } else if (entry.version == PROFILE_VERSION) {
            if (!profileEntries.contains(entry.name)) {
                profileEntries[entry.name] = QList<ProfileEntry>();
            }

            profileEntries[entry.name].append(entry);
        }
    }

    {
        const QString userLocalSaveLocation = KoResourcePaths::saveLocation("data", "input/");
        auto entriesIt = profileEntriesToMigrate.begin();
        while (entriesIt != profileEntriesToMigrate.end()) {
            ProfileEntry entry = *entriesIt;

            if (profileEntries.contains(entry.name)) {

                ProfileEntry existingEntry = profileEntries[entry.name].first();

                if (existingEntry.fullpath.startsWith(userLocalSaveLocation)) {
                    entriesIt = profileEntriesToMigrate.erase(entriesIt);
                } else {
                    entriesIt++;

                    profileEntries.remove(existingEntry.name);
                }
            } else {
                entriesIt++;
            }
        }
    }

    {
        KisInputProfileMigrator5To6 migrator(this);
        QMap<ProfileEntry, QList<KisShortcutConfiguration>> parsedProfilesToMigrate =
            migrator.migrate(profileEntriesToMigrate);

        for (ProfileEntry profileEntry : parsedProfilesToMigrate.keys()) {
            const QString storagePath = KoResourcePaths::saveLocation("data", "input/", true);

            {
                const QString profilePath = profileEntry.fullpath;
                QString oldProfileName = QFileInfo(profilePath).fileName();
                oldProfileName.replace(".profile", QString::number(PROFILE_VERSION - 1) + ".profile");

                QString oldProfilePath = storagePath + oldProfileName;
                QFile::copy(profilePath, oldProfilePath);

                KConfig config(oldProfilePath, KConfig::SimpleConfig);
                config.group("General").writeEntry("migrated", PROFILE_VERSION);
            }

            KisInputProfile *newProfile = addProfile(profileEntry.name);
            QList<KisShortcutConfiguration> shortcuts = parsedProfilesToMigrate.value(profileEntry);
            for (const auto &shortcut : shortcuts) {
                newProfile->addShortcut(new KisShortcutConfiguration(shortcut));
            }

            saveProfile(newProfile, storagePath);
        }
    }

    Q_FOREACH(const QString & profileName, profileEntries.keys()) {

        if (profileEntries[profileName].isEmpty()) {
            continue;
        }

        ProfileEntry entry = profileEntries[profileName].first();

        KConfig config(entry.fullpath, KConfig::SimpleConfig);

        KisInputProfile *newProfile = addProfile(entry.name);
        Q_FOREACH(KisAbstractInputAction * action, d->actions) {
            if (!config.hasGroup(action->id())) {
                continue;
            }

            KConfigGroup grp = config.group(action->id());
            Q_FOREACH(const QString & entry, grp.entryMap()) {
                KisShortcutConfiguration *shortcut = new KisShortcutConfiguration;
                shortcut->setAction(action);

                if (shortcut->unserialize(entry)) {
                    newProfile->addShortcut(shortcut);
                }
                else {
                    delete shortcut;
                }
            }
        }
    }

    QString currentProfile = cfg.currentInputProfile();
    if (d->profiles.size() > 0) {
        if (currentProfile.isEmpty() || !d->profiles.contains(currentProfile)) {
            QString kritaDefault = QStringLiteral("Krita Default");
            if (d->profiles.contains(kritaDefault)) {
                d->currentProfile = d->profiles.value(kritaDefault);
            } else {
                d->currentProfile = d->profiles.begin().value();
            }
        }
        else {
            d->currentProfile = d->profiles.value(currentProfile);
        }
    }
    if (d->currentProfile) {
        Q_EMIT currentProfileChanged();
    }
}

void KisInputProfileManager::saveProfiles()
{
    QString storagePath = KoResourcePaths::saveLocation("data", "input/", true);
    Q_FOREACH(KisInputProfile * p, d->profiles) {
        saveProfile(p, storagePath);
    }

    KisConfig config(false);
    config.setCurrentInputProfile(d->currentProfile->name());

    Q_EMIT currentProfileChanged();
}

void KisInputProfileManager::saveProfile(KisInputProfile *profile, QString storagePath)
{
    const QString profilePath = storagePath + d->profileFileName(profile->name());
    KConfig config(profilePath, KConfig::SimpleConfig);

    config.group("General").writeEntry("name", profile->name());
    config.group("General").writeEntry("version", PROFILE_VERSION);

    Q_FOREACH(KisAbstractInputAction * action, d->actions) {
        KConfigGroup grp = config.group(action->id());
        grp.deleteGroup();

        int index = 0;
        QList<KisShortcutConfiguration *> shortcuts = profile->shortcutsForAction(action);
        Q_FOREACH(KisShortcutConfiguration * shortcut, shortcuts) {
            grp.writeEntry(QString("%1").arg(index++), shortcut->serialize());
        }
    }

    config.sync();
}

QList<KisShortcutConfiguration *> KisInputProfileManager::getConflictingShortcuts(KisInputProfile *profile)
{
    QSet<KisShortcutConfiguration *> conflictedShortcuts;
    const QList<KisShortcutConfiguration *> shortcuts = profile->allShortcuts();
    for (auto startIt = shortcuts.constBegin(); startIt != shortcuts.constEnd(); ++startIt) {
        KisShortcutConfiguration *first = *startIt;
        for (auto index = startIt + 1; index != shortcuts.constEnd(); ++index) {
            KisShortcutConfiguration *second = *index;
            if (*first == *second && !first->isNoOp()) {
                conflictedShortcuts.insert(first);
                conflictedShortcuts.insert(second);
            }
        }
    }
    return conflictedShortcuts.values();
}

void KisInputProfileManager::resetAll()
{
    QString kdeHome = KoResourcePaths::getAppDataLocation();
    QStringList profiles = KoResourcePaths::findAllAssets("data", "input/*", KoResourcePaths::Recursive);

    Q_FOREACH (const QString &profile, profiles) {
        if(profile.contains(kdeHome)) {
            QFile::remove(profile);
        }
    }

    loadProfiles();

    Q_EMIT profilesChanged();
}

KisInputProfileManager::KisInputProfileManager(QObject *parent)
    : QObject(parent), d(new Private())
{
    d->createActions();
}

KisInputProfileManager::~KisInputProfileManager()
{
    qDeleteAll(d->profiles);
    qDeleteAll(d->actions);
    delete d;
}

void KisInputProfileManager::Private::createActions()
{
    actions.append(new KisToolInvocationAction());
    actions.append(new KisAlternateInvocationAction());
    actions.append(new KisChangePrimarySettingAction());
    actions.append(new KisPanAction());
    actions.append(new KisRotateCanvasAction());
    actions.append(new KisZoomAction());
    actions.append(new KisPopupWidgetAction());
    actions.append(new KisSelectLayerAction());
    actions.append(new KisGammaExposureAction());
    actions.append(new KisChangeFrameAction());
    actions.append(new KisZoomAndRotateAction());
    actions.append(new KisTouchGestureAction());
}

QString KisInputProfileManager::Private::profileFileName(const QString &profileName)
{
    QRegularExpression reg("[^a-z0-9]");
    return profileName.toLower().remove(reg).append(".profile");
}
