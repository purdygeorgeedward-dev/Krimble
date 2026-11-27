/*
 *  SPDX-FileCopyrightText: 2013 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_OPERATION_UI_WIDGET_FACTORY_H
#define KIS_OPERATION_UI_WIDGET_FACTORY_H

#include <KoDialog.h>
#include <klocalizedstring.h>

#include "kis_operation_ui_factory.h"
#include "operations/kis_operation_ui_widget.h"
#include "KisViewManager.h"

template <class T> class KisOperationUIWidgetFactory : public KisOperationUIFactory
{

public:
    KisOperationUIWidgetFactory(const QString &id)
        : KisOperationUIFactory(id)
        , m_configuration(nullptr)
    {
    }

    ~KisOperationUIWidgetFactory() override
    {
    }

    bool fetchConfiguration(KisViewManager* view, KisOperationConfigurationSP configuration) override {
        KoDialog * dialog = new KoDialog(view->mainWindowAsQWidget());
        Q_CHECK_PTR(dialog);

        T* configWidget = new T(dialog, view, m_configuration ? m_configuration : configuration);
        dialog->setCaption(configWidget->caption());
        dialog->setMainWidget(configWidget);
        bool success = false;
        if (dialog->exec() == QDialog::Accepted) {
            configWidget->getConfiguration(configuration);
            m_configuration = configuration;
            success = true;
        }
        delete dialog;

        return success;
    }

private:
    KisOperationConfigurationSP m_configuration;
};

#endif
