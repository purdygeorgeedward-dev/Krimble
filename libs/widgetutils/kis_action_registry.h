/*
 *  SPDX-FileCopyrightText: 2015 Michael Abrahams <miabraha@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_ACTION_REGISTRY_H
#define KIS_ACTION_REGISTRY_H

#include <QString>
#include <QKeySequence>
#include <QDomElement>
#include <QAction>
#include <QList>

#include "kritawidgetutils_export.h"

class KisKActionCollection;
class QDomElement;
class KConfigBase;
class KisShortcutsDialog;

class KRITAWIDGETUTILS_EXPORT KisActionRegistry : public QObject
{
    Q_OBJECT

public:
    static KisActionRegistry *instance();

    bool hasAction(const QString &name) const;

    QString getActionProperty(const QString &name, const QString &property);

    QAction *makeQAction(const QString &name, QObject *parent = 0);

    bool propertizeAction(const QString &name, QAction *a);

    void settingsPageSaved();

    void loadCustomShortcuts();

    void notifySettingsUpdated();

    void applyShortcutScheme(const KConfigBase *config = 0);

    struct ActionCategory {
        ActionCategory();
        ActionCategory(const QString &_componentName, const QString &_categoryName);
        QString componentName;
        QString categoryName;

        bool isValid() const;

    private:
        bool m_isValid = false;
    };

    ActionCategory fetchActionCategory(const QString &name) const;

    KisActionRegistry();
    ~KisActionRegistry();

    void loadShortcutScheme(const QString &schemeName);

    void updateShortcut(const QString &name, QAction *ac);

    bool sanityCheckPropertized(const QString &name);

    QList<QString> registeredShortcutIds() const;

Q_SIGNALS:
    void shortcutsUpdated();

private:

    class Private;
    const QScopedPointer<Private> d;
};

#endif
