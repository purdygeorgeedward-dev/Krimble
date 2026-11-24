/*
 *  SPDX-FileCopyrightText: 2013 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_OPERATION_UI_WIDGET_H
#define KIS_OPERATION_UI_WIDGET_H

#include <QWidget>
#include <kritaui_export.h>
#include "operations/kis_operation_configuration.h"

class KRITAUI_EXPORT KisOperationUIWidget : public QWidget
{

public:
    explicit KisOperationUIWidget(const QString& caption, QWidget* parent = 0);
    ~KisOperationUIWidget() override;

    QString caption() const;

    virtual void getConfiguration(KisOperationConfigurationSP config) = 0;

private:
    class Private;
    Private* const d;
};

#endif
