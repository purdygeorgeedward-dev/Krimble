/*
 * SPDX-FileCopyrightText: 2006-2016 Boudewijn Rempt (boud@valdyas.org)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_PLUGIN_LOADER_H
#define KO_PLUGIN_LOADER_H

#include <QObject>
#include <QStringList>

#include "kritaplugin_export.h"

class KPluginFactory;

#ifndef Q_MOC_RUN
#endif
class KRITAPLUGIN_EXPORT KoPluginLoader : public QObject
{

    Q_OBJECT

public:
    struct PluginsConfig {
        PluginsConfig() : group(nullptr), blacklist(nullptr) {}
        const char * group {nullptr };
        const char * blacklist {nullptr};

        inline bool isValid() const {
            return group && blacklist;
        }
    };

    ~KoPluginLoader() override;

    static KoPluginLoader * instance();

    void load(const QString & serviceType, const PluginsConfig &config = PluginsConfig(), QObject* owner = 0, bool cache = true);

    KPluginFactory* loadSinglePlugin(const std::vector<std::pair<QString, QString>> &predicates, const QString & serviceType);

    KPluginFactory* loadSinglePlugin(const std::pair<QString, QString> &predicates, const QString & serviceType);

    KPluginFactory* loadSinglePlugin(const QString &id, const QString & serviceType);

public:
    KoPluginLoader();
private:
    KoPluginLoader(const KoPluginLoader&);
    KoPluginLoader operator=(const KoPluginLoader&);

private:
    class Private;
    Private * const d;
};

#endif
