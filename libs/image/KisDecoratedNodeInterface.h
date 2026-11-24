/*
 *  SPDX-FileCopyrightText: 2019 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISDECORATEDNODEINTERFACE_H
#define KISDECORATEDNODEINTERFACE_H

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisDecoratedNodeInterface
{
public:
    virtual ~KisDecoratedNodeInterface();

    virtual bool decorationsVisible() const = 0;

    virtual void setDecorationsVisible(bool value, bool update) = 0;

    void setDecorationsVisible(bool value);
};

#endif
