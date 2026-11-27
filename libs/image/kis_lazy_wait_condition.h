/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_LAZY_WAIT_CONDITION_H
#define __KIS_LAZY_WAIT_CONDITION_H

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

class KisLazyWaitCondition
{
public:
    KisLazyWaitCondition()
        : m_waitCounter(0),
          m_wakeupCounter(0)
    {
    }

    void initWaiting() {
        QMutexLocker locker(&m_mutex);
        if(!m_waitCounter) {
            m_wakeupCounter = 0;
        }

        m_waitCounter++;
    }

    void endWaiting() {
        QMutexLocker locker(&m_mutex);
        m_waitCounter--;
    }

    bool wait(unsigned long time = ULONG_MAX) {
        QMutexLocker locker(&m_mutex);
        bool result = true;
        if(!m_wakeupCounter) {
            result = m_condition.wait(&m_mutex, time);
        }
        if(result) {
            m_wakeupCounter--;
        }
        return result;
    }

    void wakeAll() {
        if(!m_waitCounter) return;

        QMutexLocker locker(&m_mutex);
        if(m_waitCounter) {
            m_wakeupCounter += m_waitCounter;
            m_condition.wakeAll();
        }
    }

    bool isSomeoneWaiting() {
        return m_waitCounter;
    }

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    volatile int m_waitCounter;
    int m_wakeupCounter;
};

#endif
