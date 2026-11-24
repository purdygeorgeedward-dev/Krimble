/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISREQUIREDRESOURCESOPERATORS_H
#define KISREQUIREDRESOURCESOPERATORS_H

#include "kritaresources_export.h"

#include <KisResourcesInterface.h>
#include <KoResourceLoadResult.h>
#include "kis_assert.h"

#include "kis_pointer_utils.h"

namespace KisRequiredResourcesOperators
{

namespace detail {
bool KRITARESOURCES_EXPORT isLocalResourcesStorage(KisResourcesInterfaceSP resourcesInterface);
void KRITARESOURCES_EXPORT assertInGuiThread();
KisResourcesInterfaceSP KRITARESOURCES_EXPORT createLocalResourcesStorage(const QList<KoResourceSP> &resources);
void KRITARESOURCES_EXPORT addResourceOrWarnIfNotLoaded(KoResourceLoadResult loadedResource, QList<KoResourceSP> *resources, KisResourcesInterfaceSP resourcesInterface);
}

template <typename T>
bool hasLocalResourcesSnapshot(const T *object)
{
    return detail::isLocalResourcesStorage(object->resourcesInterface());
}

template <typename T>
void createLocalResourcesSnapshot(T *object, KisResourcesInterfaceSP globalResourcesInterface = nullptr)
{
    detail::assertInGuiThread();

    KisResourcesInterfaceSP effectiveResourcesInterface =
        globalResourcesInterface ?
            globalResourcesInterface :
            object->resourcesInterface();

    QList<KoResourceLoadResult> loadedResources =
        object->requiredResources(effectiveResourcesInterface);

    QList<KoResourceSP> resources;

    Q_FOREACH(const KoResourceLoadResult &loadedResource, loadedResources) {
        detail::addResourceOrWarnIfNotLoaded(loadedResource, &resources, effectiveResourcesInterface);
    }

    object->setResourcesInterface(detail::createLocalResourcesStorage(resources));
}

template <typename TypeSP, typename T = typename KisSharedPointerTraits<TypeSP>::ValueType>
TypeSP cloneWithResourcesSnapshot(const T* object,
                                  KisResourcesInterfaceSP globalResourcesInterface = nullptr)
{
    auto clonedStorage = object->clone();
    TypeSP cloned = KisSharedPointerTraits<TypeSP>::template dynamicCastSP<T>(clonedStorage);

    if (!hasLocalResourcesSnapshot(cloned.data())) {
        createLocalResourcesSnapshot(cloned.data(), globalResourcesInterface);
        KIS_SAFE_ASSERT_RECOVER_NOOP(hasLocalResourcesSnapshot(cloned.data()));
    }

    return cloned;
}

}

#endif
