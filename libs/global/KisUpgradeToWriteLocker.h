/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISUPGRADETOWRITELOCKER_H
#define KISUPGRADETOWRITELOCKER_H

#include <QReadLocker>
#include <QReadWriteLock>

class KisUpgradeToWriteLocker
{
public:
    KisUpgradeToWriteLocker(QReadLocker *locker)
        : m_locker(locker)
    {
        m_locker->unlock();
        m_locker->readWriteLock()->lockForWrite();
    }

    ~KisUpgradeToWriteLocker() {
        m_locker->readWriteLock()->unlock();
        m_locker->relock();
    }

private:
    QReadLocker *m_locker;
};

#endif
