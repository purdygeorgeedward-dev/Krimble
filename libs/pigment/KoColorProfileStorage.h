/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOCOLORPROFILESTORAGE_H
#define KOCOLORPROFILESTORAGE_H

#include <QScopedPointer>
#include "KoColorProfileConstants.h"

class QByteArray;
class QString;
class KoColorProfile;
class KoColorSpaceFactory;
struct KoColorProfileQuery;

class KoColorProfileStorage
{
public:
    KoColorProfileStorage();
    ~KoColorProfileStorage();

    void addProfile(KoColorProfile* profile);

    void removeProfile(KoColorProfile* profile);

    bool containsProfile(const KoColorProfile *profile);

    void addProfileAlias(const QString& name, const QString& to);

    void removeProfileAlias(const QString &name);

    QString profileAlias(const QString& name) const;

    const KoColorProfile * profileByName(const QString & name) const ;

    const KoColorProfile *profileByUniqueId(const QByteArray &id) const;

    QList<const KoColorProfile *>  profilesFor(const KoColorSpaceFactory * csf) const;
    QList<const KoColorProfile *> profilesFor(const KoColorProfileQuery &query,
                                              double error = 0.0001);

private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif
