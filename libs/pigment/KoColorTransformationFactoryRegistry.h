/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KOCOLORTRANSFORMATIONFACTORYREGISTRY_H
#define KOCOLORTRANSFORMATIONFACTORYREGISTRY_H

#include <KoGenericRegistry.h>

#include "kritapigment_export.h"

class KoColorSpace;
class KoColorTransformationFactory;

class KRITAPIGMENT_EXPORT KoColorTransformationFactoryRegistry : private KoGenericRegistry<KoColorTransformationFactory*>
{
    friend class KoColorSpace;
public:
    ~KoColorTransformationFactoryRegistry() override;
    static void addColorTransformationFactory(KoColorTransformationFactory* factory);
    static void removeColorTransformationFactory(KoColorTransformationFactory* factory);
private:
    static KoColorTransformationFactoryRegistry* instance();
private:
    KoColorTransformationFactoryRegistry();
private:
    struct Private;
    Private* const d;
};

#endif
