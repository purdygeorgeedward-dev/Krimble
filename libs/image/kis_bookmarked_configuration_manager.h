/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_BOOKMARKED_CONFIGURATION_MANAGER_H_
#define _KIS_BOOKMARKED_CONFIGURATION_MANAGER_H_

#include <QList>
#include "kis_serializable_configuration.h"

class QString;
class KLocalizedString;

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisBookmarkedConfigurationManager
{
public:
    static const char ConfigDefault[];
    static const char ConfigLastUsed[];
public:
    KisBookmarkedConfigurationManager(const QString & configEntryGroup, KisSerializableConfigurationFactory*);
    ~KisBookmarkedConfigurationManager();
    KisSerializableConfigurationSP load(const QString & configname) const;
    void save(const QString & configname, const KisSerializableConfigurationSP);
    bool exists(const QString & configname) const;
    QList<QString> configurations() const;
    KisSerializableConfigurationSP defaultConfiguration() const;
    void remove(const QString & name);
    QString uniqueName(const KLocalizedString & base);

private:
    QString configEntryGroup() const;
private:
    struct Private;
    Private* const d;
};

#endif
