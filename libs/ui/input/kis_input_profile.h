/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISINPUTPROFILE_H
#define KISINPUTPROFILE_H

#include <QObject>
#include <QMetaType>

class KisAbstractInputAction;
class KisShortcutConfiguration;
class KisInputProfile : public QObject
{
    Q_OBJECT

public:
    KisInputProfile(QObject *parent = 0);
    ~KisInputProfile() override;

    QString name() const;

    QList<KisShortcutConfiguration *> allShortcuts() const;
    QList<KisShortcutConfiguration *> shortcutsForAction(KisAbstractInputAction *action) const;

    void addShortcut(KisShortcutConfiguration *shortcut);
    void removeShortcut(KisShortcutConfiguration *shortcut);

public Q_SLOTS:
    void setName(const QString &name);

Q_SIGNALS:
    void nameChanged();

private:
    class Private;
    Private *const d;
};

#endif
