/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECONTROLLER_H
#define KOSHAPECONTROLLER_H

#include "kritaflake_export.h"

#include <QObject>
#include <QList>
#include <QMetaType>

class KoCanvasBase;
class KoShape;
class KoShapeContainer;
class KoShapeControllerBase;
class KUndo2Command;
class KoDocumentResourceManager;

class KRITAFLAKE_EXPORT KoShapeController : public QObject
{
    Q_OBJECT

public:
    KoShapeController(KoCanvasBase *canvas, KoShapeControllerBase *shapeController);
    ~KoShapeController() override;

    void reset();

    KUndo2Command* addShape(KoShape *shape, KoShapeContainer *parentShape, KUndo2Command *parent = 0);

    KUndo2Command* addShapeDirect(KoShape *shape, KoShapeContainer *parentShape, KUndo2Command *parent = 0);

    KUndo2Command* addShapesDirect(const QList<KoShape*> shape, KoShapeContainer *parentShape, KUndo2Command *parent = 0);

    KUndo2Command* removeShape(KoShape *shape, KUndo2Command *parent = 0);

    KUndo2Command* removeShapes(const QList<KoShape*> &shapes, KUndo2Command *parent = 0);

    void setShapeControllerBase(KoShapeControllerBase *shapeController);

    QRectF documentRectInPixels() const;

    qreal pixelsPerInch() const;

    QRectF documentRect() const;

    KoDocumentResourceManager *resourceManager() const;

    KoShapeControllerBase *documentBase() const;

private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(KoShapeController *)

#endif
