/*
 *  SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_signal_compressor.h"

#include <QTimer>
#include "kis_assert.h"
#include "kis_debug.h"

KisSignalCompressor::KisSignalCompressor()
    : QObject(0)
    , m_timer(new QTimer(this))
{
    m_timer->setSingleShot(false);
    connect(m_timer, SIGNAL(timeout()), SLOT(slotTimerExpired()));
}

KisSignalCompressor::KisSignalCompressor(int delay, Mode mode, QObject *parent)
    : KisSignalCompressor(delay, mode, PRECISE_INTERVAL, parent)
{
}

KisSignalCompressor::KisSignalCompressor(int delay, Mode mode, SlowHandlerMode slowHandlerMode, QObject *parent)
    : QObject(parent),
      m_timer(new QTimer(this)),
      m_mode(mode),
      m_slowHandlerMode(slowHandlerMode),
      m_timeout(delay)
{
    m_timer->setSingleShot(false);
    m_timer->setInterval(delay);
    connect(m_timer, SIGNAL(timeout()), SLOT(slotTimerExpired()));
}

void KisSignalCompressor::setDelayImpl(int delay)
{
    const bool wasActive = m_timer->isActive();

    if (wasActive) {
        m_timer->stop();
    }

    m_timer->setInterval(delay);

    if (wasActive) {
        m_timer->start();
    }
}

void KisSignalCompressor::setDelay(int delay)
{
    m_timeout = delay;
    m_idleCallback = {};
    setDelayImpl(delay);
}

void KisSignalCompressor::setDelay(std::function<bool ()> idleCallback, int idleDelay, int timeout)
{
    m_timeout = timeout;
    m_idleCallback = idleCallback;
    setDelayImpl(idleDelay);
}

void KisSignalCompressor::start()
{
    KIS_SAFE_ASSERT_RECOVER_RETURN(m_mode != UNDEFINED);

    const bool isFirstStart = !m_timer->isActive();

    KIS_SAFE_ASSERT_RECOVER_NOOP(!isFirstStart || !m_signalsPending);
    m_sanityIsStarting++;

    switch (m_mode) {
    case POSTPONE:
        if (isFirstStart) {
            m_timer->start();
        }
        m_lastEmittedTimer.restart();
        m_signalsPending = true;
        break;
    case FIRST_ACTIVE_POSTPONE_NEXT:
    case FIRST_ACTIVE:
        if (isFirstStart) {
            m_timer->start();
            if (m_slowHandlerMode == PRECISE_INTERVAL) {
                m_lastEmittedTimer.restart();
            }
            m_signalsPending = false;
            if (!tryEmitSignalSafely()) {
                m_signalsPending = true;
            }
            if (m_slowHandlerMode == ADDITIVE_INTERVAL) {
                m_lastEmittedTimer.restart();
            }
        } else {
            if (m_mode == FIRST_ACTIVE) {
                m_signalsPending = true;
                tryEmitOnTick(false);
            } else {
                m_lastEmittedTimer.restart();
                m_signalsPending = true;
            }
        }
        break;
    case FIRST_INACTIVE:
        if (isFirstStart) {
            m_timer->start();
            m_lastEmittedTimer.restart();
            m_signalsPending = true;
        } else {
            m_signalsPending = true;
            tryEmitOnTick(false);
        }
    case UNDEFINED:
        ;
    };

    m_sanityIsStarting--;

    KIS_SAFE_ASSERT_RECOVER(m_timer->isActive()) {
        m_timer->start();
    }
}

bool KisSignalCompressor::tryEmitOnTick(bool isFromTimer)
{
    bool wasEmitted = false;

    const int realInterval = m_timeout;
    const int minInterval = realInterval < 100 ? 0.5 * realInterval : realInterval;

    if (m_signalsPending &&
            (m_lastEmittedTimer.elapsed() >= minInterval ||
             (m_idleCallback && m_idleCallback()))) {

        KIS_SAFE_ASSERT_RECOVER_NOOP(!isFromTimer || !m_isEmitting);

        if (m_slowHandlerMode == PRECISE_INTERVAL) {
            m_lastEmittedTimer.start();
        }

        m_signalsPending = false;
        if (!tryEmitSignalSafely()) {
            m_signalsPending = true;
        }

        if (m_slowHandlerMode == ADDITIVE_INTERVAL) {
            m_lastEmittedTimer.start();
        }

        wasEmitted = true;
    } else if (!isFromTimer) {
        m_signalsPending = true;
    }

    return wasEmitted;
}

bool KisSignalCompressor::tryEmitSignalSafely()
{
    bool wasEmitted = false;

    m_isEmitting++;

    if (m_isEmitting == 1) {
        Q_EMIT timeout();
        wasEmitted = true;
    }

    m_isEmitting--;

    return wasEmitted;
}

void KisSignalCompressor::slotTimerExpired()
{
    KIS_SAFE_ASSERT_RECOVER_NOOP(!m_sanityIsStarting);
    KIS_ASSERT_RECOVER_NOOP(m_mode != UNDEFINED);
    if (!tryEmitOnTick(true)) {
        const int calmDownInterval = 5 * m_timeout;

        if (!m_lastEmittedTimer.isValid() ||
            m_lastEmittedTimer.elapsed() > calmDownInterval) {

            m_timer->stop();
        }
    }
}

void KisSignalCompressor::stop()
{
    m_timer->stop();
    m_signalsPending = false;
    m_lastEmittedTimer.invalidate();
}

bool KisSignalCompressor::isActive() const
{
    return m_signalsPending && m_timer->isActive();
}

void KisSignalCompressor::setMode(KisSignalCompressor::Mode mode)
{
    m_mode = mode;
}

int KisSignalCompressor::delay() const
{
    return m_timeout;
}
