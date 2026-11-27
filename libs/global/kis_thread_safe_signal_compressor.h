/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_THREAD_SAFE_SIGNAL_COMPRESSOR_H
#define __KIS_THREAD_SAFE_SIGNAL_COMPRESSOR_H

#include <QObject>
#include "kritaglobal_export.h"

#include "kis_signal_compressor.h"

class KRITAGLOBAL_EXPORT KisThreadSafeSignalCompressor : public QObject
{
    Q_OBJECT
public:
    KisThreadSafeSignalCompressor(int delay, KisSignalCompressor::Mode mode);

    bool isActive() const;

public Q_SLOTS:
    void setDelay(int delay);
    void start();
    void stop();

Q_SIGNALS:
    void timeout();
    void internalRequestSignal();
    void internalStopSignal();
    void internalSetDelay(int delay);

private:
    KisSignalCompressor *m_compressor;
};

#endif
