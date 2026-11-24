/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISGLOBALRESOURCESINTERFACE_H
#define KISGLOBALRESOURCESINTERFACE_H

#include "kritaresources_export.h"
#include "KisResourcesInterface.h"

class KRITARESOURCES_EXPORT KisGlobalResourcesInterface : public KisResourcesInterface
{
public:
    static KisResourcesInterfaceSP instance();

protected:
    ResourceSourceAdapter* createSourceImpl(const QString &type) const override;
};

#endif
