/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006, 2010 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2006, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCANVASBASE_H
#define KOCANVASBASE_H

#include <QPoint>

#include "kritaflake_export.h"

class KUndo2Command;

class KoUnit;
class KoCanvasResourceProvider;
class KoShapeManager;
class KoToolProxy;
class KoViewConverter;
class KoShapeController;
class KoShapeControllerBase;
class KoCanvasController;
class KoShape;
class KoSnapGuide;
class KoSelectedShapesProxy;
class KoColorDisplayRendererInterface;

class QWidget;
class QCursor;
class QPointF;
class QRectF;
class QSizeF;

#include <QObject>

class KRITAFLAKE_EXPORT KoCanvasBase : public QObject
{
    Q_OBJECT
public:

    explicit KoCanvasBase(KoShapeControllerBase *shapeController, KoCanvasResourceProvider *sharedResourceManager = 0);
    ~KoCanvasBase() override;

public:

    virtual bool canvasIsOpenGL() const { return false; }

    virtual void gridSize(QPointF *offset, QSizeF *spacing) const = 0;

    virtual bool snapToGrid() const = 0;

    virtual void setCursor(const QCursor &cursor) = 0;

    virtual void addCommand(KUndo2Command *command) = 0;

    virtual KoShapeManager *shapeManager() const = 0;

    virtual KoSelectedShapesProxy *selectedShapesProxy() const = 0;

    virtual KoShape *currentShapeManagerOwnerShape() const {
        return nullptr;
    }

    virtual void setCurrentShapeManagerOwnerShape(KoShape *source) {
        Q_UNUSED(source)
    }
    virtual void updateCanvas(const QRectF &rc) = 0;

    virtual KoToolProxy *toolProxy() const = 0;

    virtual const KoViewConverter *viewConverter() const = 0;
    virtual KoViewConverter *viewConverter() = 0;

    virtual KoColorDisplayRendererInterface* displayRendererInterface() const;

    virtual QPointF viewToDocument(const QPointF &viewPoint) const;

    virtual QWidget *canvasWidget() = 0;

    virtual const QWidget *canvasWidget() const = 0;

    virtual KoUnit unit() const = 0;

    virtual void clipToDocument(const KoShape *shape, QPointF &move) const;

    virtual QPoint documentOrigin() const {
        return QPoint(0, 0);
    }

    virtual void disconnectCanvasObserver(QObject *object);

    KoCanvasResourceProvider *resourceManager() const;

    KoShapeController *shapeController() const;

    KoCanvasController *canvasController() const;

    KoSnapGuide *snapGuide() const;

    void setCanvasController(KoCanvasController *controller);

private:
    KoCanvasBase();

    class Private;
    Private * const d;
};

#endif
