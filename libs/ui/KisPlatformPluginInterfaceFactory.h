/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISPLATFORMPLUGININTERFACEFACTORY_H
#define KISPLATFORMPLUGININTERFACEFACTORY_H

#include <kritaui_export.h>

#include <config-use-surface-color-management-api.h>

class QString;
class QWidget;
class QWindow;

class KisSurfaceColorManagerInterface;
class KisExtendedModifiersMapperPluginInterface;

#if KRITA_USE_SURFACE_COLOR_MANAGEMENT_API
class KisSurfaceColorManagementInfo;
#endif

class KRITAUI_EXPORT KisPlatformPluginInterfaceFactory {
public:
    KisPlatformPluginInterfaceFactory();

    static KisPlatformPluginInterfaceFactory* instance();

#if KRITA_USE_SURFACE_COLOR_MANAGEMENT_API
    KisSurfaceColorManagerInterface *createSurfaceColorManager(QWindow *nativeWindow);
#endif

    bool surfaceColorManagedByOS();

    QString colorManagementReport(QWidget *widget);

    QString osPreferredColorSpaceReport(QWidget *widget);

    KisExtendedModifiersMapperPluginInterface* createExtendedModifiersMapper();

private:
    bool m_surfaceColorManagedByOS {false};
};

#endif
