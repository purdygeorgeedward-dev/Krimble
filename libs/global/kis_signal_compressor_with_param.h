/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SIGNAL_COMPRESSOR_WITH_PARAM_H
#define __KIS_SIGNAL_COMPRESSOR_WITH_PARAM_H

#include <kis_signal_compressor.h>
#include <functional>

class KRITAGLOBAL_EXPORT SignalToFunctionProxy : public QObject
{
    Q_OBJECT
public:
    using TrivialFunction = std::function<void ()>;

public:
    SignalToFunctionProxy(TrivialFunction function)
        : m_function(function)
    {
    }

public Q_SLOTS:
    void start() {
        m_function();
    }

private:
    TrivialFunction m_function;
};

class KRITAGLOBAL_EXPORT FunctionToSignalProxy : public QObject
{
    Q_OBJECT

public:
    void start() {
        Q_EMIT timeout();
    }

Q_SIGNALS:
    void timeout();
};

template <typename T>
class KisSignalCompressorWithParam
{
public:
    using CallbackFunction = std::function<void (T)>;

public:
KisSignalCompressorWithParam(int delay, CallbackFunction function, KisSignalCompressor::Mode mode = KisSignalCompressor::FIRST_ACTIVE)
        : m_compressor(delay, mode),
          m_function(function)
    {
        std::function<void ()> callback(
            std::bind(&KisSignalCompressorWithParam<T>::fakeSlotTimeout, this));
        m_signalProxy.reset(new SignalToFunctionProxy(callback));

        m_compressor.connect(&m_compressor, SIGNAL(timeout()), m_signalProxy.data(), SLOT(start()));
    }

    ~KisSignalCompressorWithParam()
    {
    }

    void start(T param) {
        m_currentParamValue = param;
        m_compressor.start();
    }

    void stop() {
        m_compressor.stop();
    }

    bool isActive() const {
        return m_compressor.isActive();
    }

    void setDelay(int value) {
        m_compressor.setDelay(value);
    }

private:
    void fakeSlotTimeout() {
        m_function(m_currentParamValue);
    }

private:
    KisSignalCompressor m_compressor;
    CallbackFunction m_function;
    QScopedPointer<SignalToFunctionProxy> m_signalProxy;
    T m_currentParamValue;
};

#endif
