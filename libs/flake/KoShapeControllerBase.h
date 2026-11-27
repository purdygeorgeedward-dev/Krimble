/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOshapeControllerBASE_H
#define KOshapeControllerBASE_H

#include "kritaflake_export.h"

#include <QDomDocument>
#include <QList>

class QRectF;
class KoShape;
class KoShapeContainer;
class KoshapeControllerBasePrivate;
class KoDocumentResourceManager;
class KUndo2Command;

class KRITAFLAKE_EXPORT KoShapeControllerBase
{
public:
    KoShapeControllerBase();
    virtual ~KoShapeControllerBase();

    virtual KoShapeContainer* createParentForShapes(const QList<KoShape*> shapes, bool forceNewLayer, KUndo2Command *parentCommand);

    virtual KoDocumentResourceManager *resourceManager() const;

    virtual QRectF documentRectInPixels() const = 0;

    QRectF documentRect() const;

    virtual qreal pixelsPerInch() const = 0;

private:
    KoshapeControllerBasePrivate * const d;
};

#endif
