/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt (boud@valdyas.org)
 * SPDX-FileCopyrightText: 2006, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEREGISTRY_H
#define KOSHAPEREGISTRY_H

#include <KoGenericRegistry.h>
#include <KoShapeFactoryBase.h>
#include <QDomDocument>

#include "kritaflake_export.h"

class KoShape;
class KoShapeLoadingContext;

class KRITAFLAKE_EXPORT KoShapeRegistry : public KoGenericRegistry<KoShapeFactoryBase*>
{
public:
    KoShapeRegistry();
    ~KoShapeRegistry() override;

    static KoShapeRegistry *instance();

    void addFactory(KoShapeFactoryBase *factory);

    QList<KoShapeFactoryBase*> factoriesForElement(const QString &nameSpace, const QString &elementName);

private:
    KoShapeRegistry(const KoShapeRegistry&);
    KoShapeRegistry operator=(const KoShapeRegistry&);

    class Private;
    Private * const d;
};

#endif
