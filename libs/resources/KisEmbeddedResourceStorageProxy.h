/*
 *  SPDX-FileCopyrightText: 2022 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISEMBEDDEDRESOURCESTORAGEPROXY_H
#define KISEMBEDDEDRESOURCESTORAGEPROXY_H

#include "kritaresources_export.h"
#include "KisResourceModel.h"
#include "KisLocalStrokeResources.h"

struct KRITARESOURCES_EXPORT KisEmbeddedResourceStorageProxy {
    KisEmbeddedResourceStorageProxy(const QString &storageLocation);

    void addResource(KoResourceSP resource);;

    KisResourcesInterfaceSP resourcesInterface();

    KisResourcesInterfaceSP detachedResourcesInterface();

private:
    QString m_storageLocation;
    QSharedPointer<KisLocalStrokeResources> m_fallbackResourcesInterface;
    KisResourcesInterfaceSP m_rootResourcesInterface;

    KisResourceModel m_stylesModel;
    KisResourceModel m_patternsModel;
    KisResourceModel m_gradientsModel;
};

#endif
