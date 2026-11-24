/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ACYCLIC_SIGNAL_CONNECTOR_H
#define __KIS_ACYCLIC_SIGNAL_CONNECTOR_H

#include <QObject>
#include "kritaglobal_export.h"
#include <mutex>

class KisAcyclicSignalConnector;
class KoColor;

#include <QVector>
#include <QPointer>

class KRITAGLOBAL_EXPORT KisAcyclicSignalConnector : public QObject
{
    Q_OBJECT
public:
    typedef std::unique_lock<KisAcyclicSignalConnector> Blocker;

public:

    KisAcyclicSignalConnector(QObject *parent = 0);
    ~KisAcyclicSignalConnector();

    void connectForwardDouble(QObject *sender, const char *signal,
                              QObject *receiver, const char *method);

    void connectBackwardDouble(QObject *sender, const char *signal,
                               QObject *receiver, const char *method);

    void connectForwardInt(QObject *sender, const char *signal,
                           QObject *receiver, const char *method);

    void connectBackwardInt(QObject *sender, const char *signal,
                            QObject *receiver, const char *method);

    void connectForwardBool(QObject *sender, const char *signal,
                            QObject *receiver, const char *method);

    void connectBackwardBool(QObject *sender, const char *signal,
                             QObject *receiver, const char *method);

    void connectForwardVoid(QObject *sender, const char *signal,
                            QObject *receiver, const char *method);

    void connectBackwardVoid(QObject *sender, const char *signal,
                             QObject *receiver, const char *method);

    void connectForwardVariant(QObject *sender, const char *signal,
                               QObject *receiver, const char *method);

    void connectBackwardVariant(QObject *sender, const char *signal,
                                QObject *receiver, const char *method);

    void connectForwardResourcePair(QObject *sender, const char *signal,
                                     QObject *receiver, const char *method);

    void connectBackwardResourcePair(QObject *sender, const char *signal,
                                     QObject *receiver, const char *method);

    void connectForwardKoColor(QObject *sender, const char *signal,
                               QObject *receiver, const char *method);

    void connectBackwardKoColor(QObject *sender, const char *signal,
                                QObject *receiver, const char *method);

    void lock();

    void unlock();

    bool isLocked() const;

    KisAcyclicSignalConnector *createCoordinatedConnector();

private:

    void coordinatedLock();

    void coordinatedUnlock();

private Q_SLOTS:
    void forwardSlotDouble(double value);
    void backwardSlotDouble(double value);

    void forwardSlotInt(int value);
    void backwardSlotInt(int value);

    void forwardSlotBool(bool value);
    void backwardSlotBool(bool value);

    void forwardSlotVoid();
    void backwardSlotVoid();

    void forwardSlotVariant(const QVariant &value);
    void backwardSlotVariant(const QVariant &value);

    void forwardSlotResourcePair(int key, const QVariant &resource);
    void backwardSlotResourcePair(int key, const QVariant &resource);

    void forwardSlotKoColor(const KoColor &value);
    void backwardSlotKoColor(const KoColor &value);

Q_SIGNALS:
    void forwardSignalDouble(double value);
    void backwardSignalDouble(double value);

    void forwardSignalInt(int value);
    void backwardSignalInt(int value);

    void forwardSignalBool(bool value);
    void backwardSignalBool(bool value);

    void forwardSignalVoid();
    void backwardSignalVoid();

    void forwardSignalVariant(const QVariant &value);
    void backwardSignalVariant(const QVariant &value);

    void forwardSignalResourcePair(int key, const QVariant &value);
    void backwardSignalResourcePair(int key, const QVariant &value);

    void forwardSignalKoColor(const KoColor &value);
    void backwardSignalKoColor(const KoColor &value);

private:
    int m_signalsBlocked;
    QVector<QPointer<KisAcyclicSignalConnector>> m_coordinatedConnectors;
    QPointer<KisAcyclicSignalConnector> m_parentConnector;
};

#endif
