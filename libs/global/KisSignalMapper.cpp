/****************************************************************************
**
** SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KFQF-Accepted-GPL OR LicenseRef-Qt-Commercial
**
****************************************************************************/

#include "KisSignalMapper.h"
#include "qhash.h"

class KisSignalMapper::Private
{
public:

    Private(KisSignalMapper *_q)
        :q(_q)
        {}

    void _q_senderDestroyed() {
        q->removeMappings(q->sender());
    }
    QHash<QObject *, int> intHash;
    QHash<QObject *, QString> stringHash;
    QHash<QObject *, QWidget*> widgetHash;
    QHash<QObject *, QObject*> objectHash;

    KisSignalMapper *q;
};

KisSignalMapper::KisSignalMapper(QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
}

KisSignalMapper::~KisSignalMapper()
{
}

void KisSignalMapper::setMapping(QObject *sender, int id)
{
    d->intHash.insert(sender, id);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void KisSignalMapper::setMapping(QObject *sender, const QString &text)
{
    d->stringHash.insert(sender, text);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void KisSignalMapper::setMapping(QObject *sender, QWidget *widget)
{
    d->widgetHash.insert(sender, widget);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void KisSignalMapper::setMapping(QObject *sender, QObject *object)
{
    d->objectHash.insert(sender, object);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

QObject *KisSignalMapper::mapping(int id) const
{
    return d->intHash.key(id);
}

QObject *KisSignalMapper::mapping(const QString &id) const
{
    return d->stringHash.key(id);
}

QObject *KisSignalMapper::mapping(QWidget *widget) const
{
    return d->widgetHash.key(widget);
}

QObject *KisSignalMapper::mapping(QObject *object) const
{
    return d->objectHash.key(object);
}

void KisSignalMapper::removeMappings(QObject *sender)
{
    d->intHash.remove(sender);
    d->stringHash.remove(sender);
    d->widgetHash.remove(sender);
    d->objectHash.remove(sender);
}

void KisSignalMapper::map() { map(sender()); }

void KisSignalMapper::map(QObject *sender)
{
    if (d->intHash.contains(sender))
        Q_EMIT mapped(d->intHash.value(sender));
    if (d->stringHash.contains(sender))
        Q_EMIT mapped(d->stringHash.value(sender));
    if (d->widgetHash.contains(sender))
        Q_EMIT mapped(d->widgetHash.value(sender));
    if (d->objectHash.contains(sender))
        Q_EMIT mapped(d->objectHash.value(sender));
}

#include "moc_KisSignalMapper.cpp"

