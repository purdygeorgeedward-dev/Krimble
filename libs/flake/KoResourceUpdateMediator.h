/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KO_RESOURCE_UPDATE_MEDIATOR_H
#define __KO_RESOURCE_UPDATE_MEDIATOR_H

#include <QScopedPointer>
#include <QSharedPointer>
#include <QObject>

#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoResourceUpdateMediator : public QObject
{
    Q_OBJECT
public:
    KoResourceUpdateMediator(int key);
    ~KoResourceUpdateMediator() override;

    int key() const;
    virtual void connectResource(QVariant sourceResource) = 0;

Q_SIGNALS:
    void sigResourceChanged(int key);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

typedef QSharedPointer<KoResourceUpdateMediator> KoResourceUpdateMediatorSP;

#endif
