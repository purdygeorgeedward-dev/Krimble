/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2004 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2022 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KO_ID_H_
#define _KO_ID_H_

#include <QDebug>
#include <QMetaType>
#include <QString>

#include <boost/optional.hpp>
#include <utility>

#include <klocalizedstring.h>
#include <KisLazyStorage.h>

#include "kritaglobal_export.h"

class KRITAGLOBAL_EXPORT KoID
{
private:
    struct TranslatedString : public QString
    {
        TranslatedString(const boost::optional<KLocalizedString> &source);

        TranslatedString(const QString &value);
    };

    using StorageType =
        KisLazyStorage<TranslatedString,
        boost::optional<KLocalizedString>>;

    struct KoIDPrivate {
        KoIDPrivate(QString _id, const KLocalizedString &_name);

        KoIDPrivate(QString _id, const QString &_name);

        QString id;
        StorageType name;
    };

public:
    KoID();

    explicit KoID(const QString &id, const QString &name = QString());

    explicit KoID(const QString &id, const KLocalizedString &name);

    KoID(const KoID &rhs);

    KoID &operator=(const KoID &rhs);

    QString id() const;

    QString name() const;

    friend inline bool operator==(const KoID &, const KoID &);
    friend inline bool operator!=(const KoID &, const KoID &);
    friend inline bool operator<(const KoID &, const KoID &);
    friend inline bool operator>(const KoID &, const KoID &);

    static bool compareNames(const KoID &id1, const KoID &id2)
    {
        return id1.name() < id2.name();
    }

private:
    QSharedPointer<KoIDPrivate> m_d;
};

Q_DECLARE_METATYPE(KoID)

inline bool operator==(const KoID &v1, const KoID &v2)
{
    return v1.m_d == v2.m_d || v1.m_d->id == v2.m_d->id;
}

inline bool operator!=(const KoID &v1, const KoID &v2)
{
    return !(v1 == v2);
}

inline bool operator<(const KoID &v1, const KoID &v2)
{
    return v1.m_d->id < v2.m_d->id;
}

inline bool operator>(const KoID &v1, const KoID &v2)
{
    return v1.m_d->id > v2.m_d->id;;
}

inline QDebug operator<<(QDebug dbg, const KoID &id)
{
    dbg.nospace() << id.name() << " (" << id.id() << " )";

    return dbg.space();
}

#endif
