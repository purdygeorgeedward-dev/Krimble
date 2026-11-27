/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPESAVINGCONTEXT_H
#define KOSHAPESAVINGCONTEXT_H

#include "kritaflake_export.h"

#include <QMap>

class KoShape;
class KoXmlWriter;
class KoMarker;
class KoShapeLayer;
class KoStore;
class KoSharedSavingData;
class KoShapeSavingContextPrivate;

class QImage;
class QTransform;

class KRITAFLAKE_EXPORT KoShapeSavingContext
{
public:
    enum ShapeSavingOption {
        PresentationShape = 1,
        DrawId = 2,
        AutoStyleInStyleXml = 4,
        UniqueMasterPages = 8,
        ZIndex = 16
    };
    Q_DECLARE_FLAGS(ShapeSavingOptions, ShapeSavingOption)

    KoShapeSavingContext(KoXmlWriter &xmlWriter);
    virtual ~KoShapeSavingContext();

    KoXmlWriter &xmlWriter();

    void setXmlWriter(KoXmlWriter &xmlWriter);

    bool isSet(ShapeSavingOption option) const;

    void setOptions(ShapeSavingOptions options);

    void addOption(ShapeSavingOption option);

    void removeOption(ShapeSavingOption option);

    ShapeSavingOptions options() const;

    void addLayerForSaving(const KoShapeLayer *layer);

    void saveLayerSet(KoXmlWriter &xmlWriter) const;

    void clearLayers();

    QMap<qint64, QString> imagesToSave();

    QString markerRef(const KoMarker *marker);

    void addSharedData(const QString &id, KoSharedSavingData *data);

    KoSharedSavingData *sharedData(const QString &id) const;

    void addShapeOffset(const KoShape *shape, const QTransform &matrix);

    void removeShapeOffset(const KoShape *shape);

    QTransform shapeOffset(const KoShape *shape) const;

private:
    KoShapeSavingContextPrivate * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoShapeSavingContext::ShapeSavingOptions)

#endif
