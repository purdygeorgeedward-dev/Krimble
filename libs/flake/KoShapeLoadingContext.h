/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPELOADINGCONTEXT_H
#define KOSHAPELOADINGCONTEXT_H

#include <QSet>
#include <QString>
#include <QPair>

#include "kritaflake_export.h"

class KoShapeLayer;
class KoShape;
class KoShapeControllerBase;
class KoLoadingShapeUpdater;
class KoSharedLoadingData;
class KoDocumentResourceManager;
class KoSectionModel;
class QVariant;
class QObject;
class KoStore;

class KRITAFLAKE_EXPORT KoShapeLoadingContext
{
public:
    struct AdditionalAttributeData {
        AdditionalAttributeData(const QString &ns, const QString &tag, const QString &name)
                : ns(ns)
                , tag(tag)
                , name(name) {
        }

        const QString ns;
        const QString tag;
        const QString name;

        bool operator==(const AdditionalAttributeData &other) const {
            return name == other.name;
        }
    };

    KoShapeLoadingContext(KoStore *store, KoDocumentResourceManager *documentResources);

    ~KoShapeLoadingContext();

    KoStore *store() const;
    QString mimeTypeForPath(const QString &href, bool b = true);

    KoShapeLayer *layer(const QString &layerName);
    void addLayer(KoShapeLayer *layer, const QString &layerName);

    void clearLayers();

    void addShapeId(KoShape *shape, const QString &id);

    KoShape *shapeById(const QString &id);

    void addShapeSubItemId(KoShape *shape, const QVariant &subItem, const QString &id);
    QPair<KoShape *, QVariant> shapeSubItemById(const QString &id);

    void updateShape(const QString &id, KoLoadingShapeUpdater *shapeUpdater);

    void shapeLoaded(KoShape *shape);

    int zIndex();

    void setZIndex(int index);

    void addSharedData(const QString &id, KoSharedLoadingData *data);

    KoSharedLoadingData *sharedData(const QString &id) const;

    static void addAdditionalAttributeData(const AdditionalAttributeData &attributeData);

    static QSet<AdditionalAttributeData> additionalAttributeData();

    KoDocumentResourceManager *documentResourceManager() const;
    KoSectionModel *sectionModel();

    void setSectionModel(KoSectionModel *sectionModel);

private:
    class Private;
    Private * const d;
};

#endif
