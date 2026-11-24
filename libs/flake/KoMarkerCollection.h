/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOMARKERCOLLECTION_H
#define KOMARKERCOLLECTION_H

#include "kritaflake_export.h"

#include <QObject>
#include <QList>
#include <QMetaType>

class KoMarker;
#include <QDomDocument>
class KoShapeLoadingContext;

class KRITAFLAKE_EXPORT KoMarkerCollection : public QObject
{
    Q_OBJECT
public:
    explicit KoMarkerCollection(QObject *parent = 0);
    ~KoMarkerCollection() override;

    QList<KoMarker*> markers() const;

    KoMarker * addMarker(KoMarker *marker);

    void loadMarkersFromFile(const QString &svgFile);

private:
    void loadDefaultMarkers();

    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(KoMarkerCollection *)

#endif
