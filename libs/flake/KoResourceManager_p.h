/*
   SPDX-FileCopyrightText: 2006, 2011 Boudewijn Rempt (boud@valdyas.org)
   SPDX-FileCopyrightText: 2007, 2009, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_RESOURCEMANAGER_P_H
#define KO_RESOURCEMANAGER_P_H

#include <QObject>
#include <QSizeF>
#include <QHash>

#include "kritaflake_export.h"
#include <KoColor.h>
#include <KoUnit.h>
#include "KoDerivedResourceConverter.h"
#include "KoResourceUpdateMediator.h"
#include "KoActiveCanvasResourceDependency.h"
#include "KoAbstractCanvasResourceInterface.h"

class KoShape;
class QVariant;

class KRITAFLAKE_EXPORT KoResourceManager : public QObject
{
    Q_OBJECT
public:

    KoResourceManager() {}

    void setResource(int key, const QVariant &value);

    void setResource(int key, const KoColor &color);

    void setResource(int key, KoShape *shape);

    void setResource(int key, const KoUnit &unit);

    QVariant resource(int key) const;

    bool boolResource(int key) const;

    int intResource(int key) const;

    KoColor koColorResource(int key) const;

    KoShape *koShapeResource(int key) const;

    QString stringResource(int key) const;

    QSizeF sizeResource(int key) const;

    KoUnit unitResource(int key) const;

    bool hasResource(int key) const;

    void clearResource(int key);

    void addDerivedResourceConverter(KoDerivedResourceConverterSP converter);

    bool hasDerivedResourceConverter(int key);

    void removeDerivedResourceConverter(int key);

    void addResourceUpdateMediator(KoResourceUpdateMediatorSP mediator);

    bool hasResourceUpdateMediator(int key);

    void removeResourceUpdateMediator(int key);

    void addActiveCanvasResourceDependency(KoActiveCanvasResourceDependencySP dep);

    bool hasActiveCanvasResourceDependency(int sourceKey, int targetKey) const;

    void removeActiveCanvasResourceDependency(int sourceKey, int targetKey);

    bool hasAbstractResource(int key);

    void removeAbstractResource(int key);

    void setAbstractResource(KoAbstractCanvasResourceInterfaceSP resource);

Q_SIGNALS:
    void resourceChanged(int key, const QVariant &value);
    void resourceChangeAttempted(int key, const QVariant &value);

private:
    void notifyResourceChanged(int key, const QVariant &value);
    void notifyDerivedResourcesChanged(int key, const QVariant &value);

    void notifyResourceChangeAttempted(int key, const QVariant &value);
    void notifyDerivedResourcesChangeAttempted(int key, const QVariant &value);

    void notifyDependenciesAboutTargetChange(int targetKey, const QVariant &value);

private Q_SLOTS:
    void slotResourceInternalsChanged(int key);
    void slotAbstractResourceChangedExternal(int key, const QVariant &value);

private:
    KoResourceManager(const KoResourceManager&);
    KoResourceManager& operator=(const KoResourceManager&);

    QHash<int, QVariant> m_resources;

    QHash<int, KoDerivedResourceConverterSP> m_derivedResources;
    QMultiHash<int, KoDerivedResourceConverterSP> m_derivedFromSource;

    QMultiHash<int, KoActiveCanvasResourceDependencySP> m_dependencyFromSource;
    QMultiHash<int, KoActiveCanvasResourceDependencySP> m_dependencyFromTarget;

    QHash<int, KoResourceUpdateMediatorSP> m_updateMediators;
    QHash<int, KoAbstractCanvasResourceInterfaceSP> m_abstractResources;
};

#endif

