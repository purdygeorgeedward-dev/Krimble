/*
 *  SPDX-FileCopyrightText: 2013 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_OPERATION_UI_FACTORY_H
#define KIS_OPERATION_UI_FACTORY_H

#include "kritaui_export.h"
#include <QString>
#include "operations/kis_operation_configuration.h"

class KisViewManager;

class KRITAUI_EXPORT KisOperationUIFactory
{
public:
    KisOperationUIFactory(const QString &id);
    virtual ~KisOperationUIFactory();

    QString id() const;

    virtual bool fetchConfiguration(KisViewManager* view, KisOperationConfigurationSP configuration) = 0;

private:
    class Private;
    Private* const d;
};

#endif
