/*
 *  SPDX-FileCopyrightText: 2018 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISINPUTACTIONGROUP_H
#define KISINPUTACTIONGROUP_H

#include <QFlags>
#include <QSharedPointer>

enum KisInputActionGroup {
    NoActionGroup = 0x0,
    ViewTransformActionGroup = 0x1,
    ModifyingActionGroup = 0x2,
    ToolInvoactionActionGroup = 0x4,
    AllActionGroup = ViewTransformActionGroup | ModifyingActionGroup | ToolInvoactionActionGroup
};

Q_DECLARE_FLAGS(KisInputActionGroupsMask, KisInputActionGroup)
Q_DECLARE_OPERATORS_FOR_FLAGS(KisInputActionGroupsMask)

class KisInputActionGroupsMaskGuard;

struct KisInputActionGroupsMaskInterface
{
    virtual ~KisInputActionGroupsMaskInterface();

    virtual KisInputActionGroupsMask inputActionGroupsMask() const = 0;

    virtual void setInputActionGroupsMask(KisInputActionGroupsMask mask) = 0;

    using SharedInterface=QSharedPointer<KisInputActionGroupsMaskInterface>;
};

class KisInputActionGroupsMaskGuard
{
public:
    KisInputActionGroupsMaskGuard(KisInputActionGroupsMaskInterface::SharedInterface sharedInterface, KisInputActionGroupsMask mask);

    ~KisInputActionGroupsMaskGuard();

private:
    KisInputActionGroupsMaskInterface::SharedInterface m_sharedInterface;
    KisInputActionGroupsMask m_oldMask;
};

#endif
