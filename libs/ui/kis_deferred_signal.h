/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_DEFERRED_SIGNAL_H
#define __KIS_DEFERRED_SIGNAL_H

#include <QObject>

#include <functional>

class KisDeferredSignal : public QObject
{
    Q_OBJECT
public:
    using CallbackFunction = std::function<void ()>;

public:
    static void deferSignal(int delay, CallbackFunction function);

private Q_SLOTS:
    void timeout();

private:
    KisDeferredSignal(int delay, CallbackFunction function);

private:
    CallbackFunction m_function;
};

#endif
