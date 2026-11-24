/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SIGNAL_AUTO_CONNECTOR_H
#define __KIS_SIGNAL_AUTO_CONNECTOR_H

#include <QObject>
#include <QSharedPointer>
#include <QVector>

class KisSignalAutoConnection
{
public:
    template<class Sender, class Signal, class Receiver, class Method>
    inline KisSignalAutoConnection(Sender sender, Signal signal,
                                  Receiver receiver, Method method,
                                  Qt::ConnectionType type = Qt::AutoConnection)
        : m_connection(QObject::connect(sender, signal, receiver, method, type))
    {
    }

    inline ~KisSignalAutoConnection()
    {
        QObject::disconnect(m_connection);
    }

private:
    KisSignalAutoConnection(const KisSignalAutoConnection &rhs);

private:
    QMetaObject::Connection m_connection;
};

typedef QSharedPointer<KisSignalAutoConnection> KisSignalAutoConnectionSP;

class KisSignalAutoConnectionsStore
{
public:
    template<class Sender, class Signal, class Receiver, class Method>
    inline void addConnection(Sender sender, Signal signal,
                              Receiver receiver, Method method,
                              Qt::ConnectionType type = Qt::AutoConnection)
    {
        m_connections.append(KisSignalAutoConnectionSP(
                                 new KisSignalAutoConnection(sender, signal,
                                                             receiver, method, type)));
    }

    template<class Sender, class Signal, class Receiver, class Method>
    inline void addUniqueConnection(Sender sender, Signal signal,
                                    Receiver receiver, Method method)
    {
        m_connections.append(KisSignalAutoConnectionSP(
                                 new KisSignalAutoConnection(sender, signal,
                                                             receiver, method, Qt::UniqueConnection)));
    }

    inline void clear() {
        m_connections.clear();
    }

    inline bool isEmpty() {
        return m_connections.isEmpty();
    }

private:
    QVector<KisSignalAutoConnectionSP> m_connections;
};

#endif
