/*
   SPDX-FileCopyrightText: 2006, 2011 Boudewijn Rempt (boud@valdyas.org)
   SPDX-FileCopyrightText: 2007, 2009, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_CANVASRESOURCEMANAGER_H
#define KO_CANVASRESOURCEMANAGER_H

#include <QObject>

#include "kritaflake_export.h"
#include "KoDerivedResourceConverter.h"
#include "KoResourceUpdateMediator.h"
#include "KoActiveCanvasResourceDependency.h"
#include <KoCanvasResourcesIds.h>
#include "KoAbstractCanvasResourceInterface.h"

template<class T> class QSharedPointer;
class KoCanvasResourcesInterface;
using KoCanvasResourcesInterfaceSP = QSharedPointer<KoCanvasResourcesInterface>;

class KoShape;
class KoShapeStroke;
class KoColor;
class KoUnit;

class QVariant;
class QSizeF;

class KRITAFLAKE_EXPORT KoCanvasResourceProvider : public QObject
{
    Q_OBJECT

public:
    enum ApplicationSpecial {
        NoSpecial = 0,
        NoAdvancedText = 1
    };

    explicit KoCanvasResourceProvider(QObject *parent = 0);
    ~KoCanvasResourceProvider() override;

public Q_SLOTS:
    void setResource(int key, const QVariant &value);

    void setResource(int key, const KoColor &color);

    void setResource(int key, KoShape *shape);

    void setResource(int key, const KoUnit &unit);

    void setAbstractResource(KoAbstractCanvasResourceInterfaceSP abstractResource);

public:
    QVariant resource(int key) const;

    void setForegroundColor(const KoColor &color);

    KoColor foregroundColor() const;

    void setBackgroundColor(const KoColor &color);
    KoColor backgroundColor() const;

    bool boolResource(int key) const;

    int intResource(int key) const;

    KoColor koColorResource(int key) const;

    KoShape *koShapeResource(int key) const;

    QString stringResource(int key) const;

    QSizeF sizeResource(int key) const;

    KoUnit unitResource(int key) const;

    void setHandleRadius(int handleSize);
    int handleRadius() const;

    void setDecorationThickness(int decorationThickness);

    int decorationThickness() const;

    void setUsingOtherColor(bool usingOtherColor);

    bool isUsingOtherColor() const;

    bool hasResource(int key) const;

    void clearResource(int key);

    void addDerivedResourceConverter(KoDerivedResourceConverterSP converter);

    bool hasDerivedResourceConverter(int key);

    void removeDerivedResourceConverter(int key);

    bool hasAbstractResource(int key);

    void removeAbstractResource(int key);

    void addResourceUpdateMediator(KoResourceUpdateMediatorSP mediator);

    bool hasResourceUpdateMediator(int key);

    void removeResourceUpdateMediator(int key);

    void addActiveCanvasResourceDependency(KoActiveCanvasResourceDependencySP dep);

    bool hasActiveCanvasResourceDependency(int sourceKey, int targetKey) const;

    void removeActiveCanvasResourceDependency(int sourceKey, int targetKey);

    KoCanvasResourcesInterfaceSP canvasResourcesInterface() const;

Q_SIGNALS:
    void canvasResourceChanged(int key, const QVariant &value);

    void canvasResourceChangeAttempted(int key, const QVariant &value);

private:
    KoCanvasResourceProvider(const KoCanvasResourceProvider&);
    KoCanvasResourceProvider& operator=(const KoCanvasResourceProvider&);

    class Private;
    Private *const d;
};

#endif
