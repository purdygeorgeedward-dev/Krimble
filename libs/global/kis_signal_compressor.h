/*
 *  SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SIGNAL_COMPRESSOR_H
#define __KIS_SIGNAL_COMPRESSOR_H

#include <QObject>
#include "kritaglobal_export.h"

#include <QElapsedTimer>
#include <functional>

class QTimer;

class KRITAGLOBAL_EXPORT KisSignalCompressor : public QObject
{
    Q_OBJECT

public:
    enum Mode {
        POSTPONE,
        FIRST_ACTIVE_POSTPONE_NEXT,
        FIRST_ACTIVE,
        FIRST_INACTIVE,
        UNDEFINED
    };

    enum SlowHandlerMode {
        PRECISE_INTERVAL,
        ADDITIVE_INTERVAL
    };

public:
    KisSignalCompressor();
    KisSignalCompressor(int delay, Mode mode, QObject *parent = 0);
    KisSignalCompressor(int delay, Mode mode, SlowHandlerMode slowHandlerMode, QObject *parent = 0);
    bool isActive() const;
    void setMode(Mode mode);

    int delay() const;

    void setDelay(std::function<bool()> idleCallback, int idleDelay, int timeout);

public Q_SLOTS:
    void setDelay(int delay);
    void start();
    void stop();

private Q_SLOTS:
    void slotTimerExpired();

Q_SIGNALS:
    void timeout();

private:
    bool tryEmitOnTick(bool isFromTimer);
    bool tryEmitSignalSafely();
    void setDelayImpl(int delay);

private:
    QTimer *m_timer = 0;
    Mode m_mode = UNDEFINED;
    SlowHandlerMode m_slowHandlerMode = PRECISE_INTERVAL;
    bool m_signalsPending = false;
    QElapsedTimer m_lastEmittedTimer;
    int m_isEmitting = 0;
    int m_timeout = 0;
    std::function<bool()> m_idleCallback;
    int m_sanityIsStarting = 0;
};

#endif
