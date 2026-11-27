/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISDATABASETRANSACTIONLOCK_H
#define KISDATABASETRANSACTIONLOCK_H

#include <QSqlDatabase>

#include <kritaresources_export.h>

#include <KisAdaptedLock.h>

namespace detail
{
struct KRITARESOURCES_EXPORT KisDatabaseTransactionLockAdapter {

    KisDatabaseTransactionLockAdapter(QSqlDatabase database);

    void lock();
    void unlock();
    void commit();

private:
    QSqlDatabase m_database;
    bool m_transactionStarted{false};
};
}

class KRITARESOURCES_EXPORT KisDatabaseTransactionLock
    : public KisAdaptedLock<detail::KisDatabaseTransactionLockAdapter>
{
public:
    using BaseClass = KisAdaptedLock<detail::KisDatabaseTransactionLockAdapter>;
    using BaseClass::BaseClass;
    using BaseClass::commit;

    inline void rollback() {
        unlock();
    }
};

#endif
