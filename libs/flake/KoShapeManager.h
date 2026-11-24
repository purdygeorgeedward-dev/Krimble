/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006-2008 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPEMANAGER_H
#define KOSHAPEMANAGER_H

#include <QList>
#include <QObject>
#include <QRect>

#include "KoFlake.h"
#include "kritaflake_export.h"

#include <memory>
#include <vector>

class KoShape;
class KoSelection;
class KoViewConverter;
class KoCanvasBase;
class KoPointerEvent;

class QPainter;
class QPointF;
class QRectF;

class KRITAFLAKE_EXPORT KoShapeManager : public QObject
{
    Q_OBJECT

public:
    enum Repaint {
        PaintShapeOnAdd,
        AddWithoutRepaint
    };

    explicit KoShapeManager(KoCanvasBase *canvas);
    KoShapeManager(KoCanvasBase *canvas, const QList<KoShape *> &shapes);
    ~KoShapeManager() override;

    void setShapes(const QList<KoShape *> &shapes, Repaint repaint = PaintShapeOnAdd);

    QList<KoShape*> shapes() const;

    QList<KoShape*> topLevelShapes() const;

public Q_SLOTS:
    void addShape(KoShape *shape, KoShapeManager::Repaint repaint = PaintShapeOnAdd);

    void remove(KoShape *shape);

public:
    KoSelection *selection() const;

    struct PaintJob {
        using ShapesStorage = std::vector<std::unique_ptr<KoShape>>;
        using SharedSafeStorage = std::shared_ptr<ShapesStorage>;

        PaintJob() = default;
        PaintJob(QRectF _docUpdateRect, QRect _viewUpdateRect)
            : docUpdateRect(_docUpdateRect),
              viewUpdateRect(_viewUpdateRect)
        {
        }

        bool isEmpty() const {
            return shapes.isEmpty();
        }

        QRectF docUpdateRect;
        QRect viewUpdateRect;

        QList<KoShape*> shapes;
        SharedSafeStorage allClonedShapes;
    };

    struct PaintJobsOrder
    {
        QRect uncroppedViewUpdateRect;
        QList<PaintJob> jobs;

        inline void clear() {
            jobs.clear();
            uncroppedViewUpdateRect = QRect();
        }

        inline bool isEmpty() const {
            return jobs.isEmpty();
        }
    };

    void explicitlyIssueShapeChangedSignals();

    void preparePaintJobs(PaintJobsOrder &jobsOrder, KoShape *excludeRoot);

    void paintJob(QPainter &painter, const KoShapeManager::PaintJob &job);

    void paint(QPainter &painter);

    KoShape *shapeAt(const QPointF &position, KoFlake::ShapeSelection selection = KoFlake::ShapeOnTop, bool omitHiddenShapes = true);

    QList<KoShape *> shapesAt(const QRectF &rect, bool omitHiddenShapes = true, bool containedMode = false);

    void update(const QRectF &rect, const KoShape *shape = 0, bool selectionHandles = false);

    void setUpdatesBlocked(bool value);

    bool updatesBlocked() const;

    void notifyShapeChanged(KoShape *shape);

    static void renderSingleShape(KoShape *shape, QPainter &painter);

    struct ShapeInterface {
        ShapeInterface(KoShapeManager *_q);

        void notifyShapeDestructed(KoShape *shape);

    protected:
        KoShapeManager *q;
    };

    ShapeInterface* shapeInterface();

Q_SIGNALS:
    void selectionChanged();
    void selectionContentChanged();
    void contentChanged();

    void forwardUpdate();

private:
    KoCanvasBase *canvas();

    class Private;
    Private * const d;
    Q_PRIVATE_SLOT(d, void updateTree())
    Q_PRIVATE_SLOT(d, void forwardCompressedUpdate())
};

#endif
