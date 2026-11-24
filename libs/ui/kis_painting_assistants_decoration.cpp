/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2017 Scott Petrovic <scottpetrovic@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_painting_assistants_decoration.h"

#include <cstdint>
#include <limits>

#include <QList>
#include <QPointF>
#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kis_algebra_2d.h>
#include "kis_debug.h"
#include "KisDocument.h"
#include "kis_canvas2.h"
#include "kis_canvas_resource_provider.h"
#include "kis_icon_utils.h"
#include "KisViewManager.h"
#include <KoCompositeOpRegistry.h>
#include "kis_tool_proxy.h"
#include <KoColorDisplayRendererInterface.h>

#include <QPainter>
#include <QPainterPath>
#include <QApplication>

struct KisPaintingAssistantsDecoration::Private {
    Private()
        : assistantVisible(false)
        , outlineVisible(false)
        , snapOnlyOneAssistant(true)
        , snapEraser(false)
        , useCache(false)
        , firstAssistant(0)
        , m_handleSize(14)
    {}

    bool assistantVisible;
    bool outlineVisible;
    bool snapOnlyOneAssistant;
    bool snapEraser;
    bool useCache;
    KisPaintingAssistantSP firstAssistant;
    KisPaintingAssistantSP selectedAssistant;
    bool m_isEditingAssistants = false;
    int m_handleSize;

    KisCanvas2 * m_canvas = 0;
};

KisPaintingAssistantsDecoration::KisPaintingAssistantsDecoration(QPointer<KisView> parent) :
    KisCanvasDecoration("paintingAssistantsDecoration", parent),
    d(new Private)
{
    setAssistantVisible(true);
    setOutlineVisible(true);
    setPriority(95);
    d->snapOnlyOneAssistant = true;
    d->snapEraser = false;

    slotConfigChanged();
}

KisPaintingAssistantsDecoration::~KisPaintingAssistantsDecoration()
{
    delete d;
}

void KisPaintingAssistantsDecoration::slotUpdateDecorationVisibility()
{
    const bool shouldBeVisible = !assistants().isEmpty();

    if (visible() != shouldBeVisible) {
        setVisible(shouldBeVisible);
    }
}

void KisPaintingAssistantsDecoration::slotConfigChanged()
{
    KisConfig cfg(true);
    const KisConfig::AssistantsDrawMode drawMode = cfg.assistantsDrawMode();

    d->useCache =
        (drawMode == KisConfig::ASSISTANTS_DRAW_MODE_PIXMAP_CACHE) ||
        (drawMode == KisConfig::ASSISTANTS_DRAW_MODE_LARGE_PIXMAP_CACHE);
}

void KisPaintingAssistantsDecoration::addAssistant(KisPaintingAssistantSP assistant)
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    if (assistants.contains(assistant)) return;

    assistants.append(assistant);
    assistant->setAssistantGlobalColorCache(view()->document()->assistantsGlobalColor());

    view()->document()->setAssistants(assistants);
    setVisible(!assistants.isEmpty());
    Q_EMIT assistantChanged();
}

void KisPaintingAssistantsDecoration::raiseAssistant(KisPaintingAssistantSP assistant)
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    assistants.removeOne(assistant);

    if (!assistant) return;
    assistants.append(assistant);

    view()->document()->setAssistants(assistants);
    setVisible(!assistants.isEmpty());
    Q_EMIT assistantChanged();

}

void KisPaintingAssistantsDecoration::removeAssistant(KisPaintingAssistantSP assistant)
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    KIS_ASSERT_RECOVER_NOOP(assistants.contains(assistant));

    if (assistants.removeAll(assistant)) {
        view()->document()->setAssistants(assistants);
        setVisible(!assistants.isEmpty());
        Q_EMIT assistantChanged();
    }
}

void KisPaintingAssistantsDecoration::removeAll()
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    assistants.clear();
    view()->document()->setAssistants(assistants);
    setVisible(!assistants.isEmpty());

    Q_EMIT assistantChanged();
}

void KisPaintingAssistantsDecoration::setAssistants(const QList<KisPaintingAssistantSP> &assistants)
{
    Q_FOREACH (KisPaintingAssistantSP assistant, assistants) {
        assistant->setAssistantGlobalColorCache(view()->document()->assistantsGlobalColor());
    }
    view()->document()->setAssistants(assistants);
    setVisible(!assistants.isEmpty());

    Q_EMIT assistantChanged();
}

void KisPaintingAssistantsDecoration::setAdjustedBrushPosition(const QPointF position)
{
    if (!assistants().empty()) {
        Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
            assistant->setAdjustedBrushPosition(position);
        }
    }
}

QPointF KisPaintingAssistantsDecoration::adjustPosition(const QPointF& point, const QPointF& strokeBegin)
{

    if (assistants().empty()) {
        return point;
    }

    if  (!d->snapEraser
        && (d->m_canvas->resourceManager()->resource(KoCanvasResource::CurrentEffectiveCompositeOp).toString() == COMPOSITE_ERASE)) {
        return point;
    }

    KisImageSP image = d->m_canvas->image();
    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(image, point);

    const KisCoordinatesConverter *converter = d->m_canvas->coordinatesConverter();
    const qreal moveThresholdPt = 4.0 / (converter->effectiveZoom() * qMax(image->xRes(), image->yRes()));

    QPointF best = point;
    qreal minSquareDistance = std::numeric_limits<qreal>::max();
    qreal secondSquareDistance = std::numeric_limits<qreal>::max();

    if (!d->snapOnlyOneAssistant || !d->firstAssistant) {
        int numSuitableAssistants = 0;
        KisPaintingAssistantSP bestAssistant;

        Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
            if (assistant->isSnappingActive() == true){
                QPointF newpoint = assistant->adjustPosition(point, strokeBegin, true, moveThresholdPt);
                if (qIsNaN(newpoint.x()) ||  qIsNaN(newpoint.y())) {
                    continue;
                }
                ++numSuitableAssistants;
                qreal dist = kisSquareDistance(newpoint, point);
                if (dist < minSquareDistance) {
                    best = newpoint;
                    secondSquareDistance = minSquareDistance;
                    minSquareDistance = dist;
                    bestAssistant = assistant;
                } else if (dist < secondSquareDistance) {
                    secondSquareDistance = dist;
                }
                assistant->setFollowBrushPosition(true);
            }
        }

        if (numSuitableAssistants > 1 && KisAlgebra2D::norm(point - strokeBegin) <= moveThresholdPt
                && (sqrt(secondSquareDistance) < moveThresholdPt)) {
            return strokeBegin;
        } else if (numSuitableAssistants > 0 && d->snapOnlyOneAssistant) {
            d->firstAssistant = bestAssistant;
        }
    } else {
        KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(d->firstAssistant->isSnappingActive(), point);
        QPointF newpoint = d->firstAssistant->adjustPosition(point, strokeBegin, false, moveThresholdPt);
        if (!(qIsNaN(newpoint.x()) || qIsNaN(newpoint.y()))) {
            best = newpoint;
        }
    }

    return best;
}

void KisPaintingAssistantsDecoration::adjustLine(QPointF &point, QPointF &strokeBegin)
{
    if (assistants().empty()) {
        return;
    }

    if  (!d->snapEraser
        && (d->m_canvas->resourceManager()->resource(KoCanvasResource::CurrentEffectiveCompositeOp).toString() == COMPOSITE_ERASE)) {
        return;
    }

    QPointF originalPoint = point;
    QPointF originalStrokeBegin = strokeBegin;

    qreal minDistance = 10000.0;
    bool minDistValid = false;
    QPointF finalPoint = originalPoint;
    QPointF finalStrokeBegin = originalStrokeBegin;
    KisPaintingAssistantSP bestAssistant;
    Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
        if(assistant->isSnappingActive() == true){
            QPointF p1 = originalPoint;
            QPointF p2 = originalStrokeBegin;
            assistant->adjustLine(p1, p2);
            if (p1.isNull() || p2.isNull()) {
                continue;
            }
            qreal distance = kisSquareDistance(p1, originalPoint) + kisSquareDistance(p2, originalStrokeBegin);
            if (distance < minDistance || !minDistValid) {
                finalPoint = p1;
                finalStrokeBegin = p2;
                minDistValid = true;
                bestAssistant = assistant;
            }
        }
    }
    if (bestAssistant) {
        bestAssistant->setFollowBrushPosition(true);
    }
    point = finalPoint;
    strokeBegin = finalStrokeBegin;
}

void KisPaintingAssistantsDecoration::endStroke()
{
    d->firstAssistant.clear();

    Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
        assistant->endStroke();
    }
}

void KisPaintingAssistantsDecoration::drawDecoration(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter, KisCanvas2* canvas)
{
    if(assistants().isEmpty()) {
        return;
    }

    if (!canvas) {
        dbgFile<<"canvas does not exist in painting assistant decoration, you may have passed arguments incorrectly:"<<canvas;
    } else {
        d->m_canvas = canvas;
    }

    KoToolProxy *proxy = view()->canvasBase()->toolProxy();
    KIS_SAFE_ASSERT_RECOVER_RETURN(proxy);
    KisToolProxy *kritaProxy = dynamic_cast<KisToolProxy*>(proxy);
    KIS_SAFE_ASSERT_RECOVER_RETURN(kritaProxy);

    const bool outlineVisible =
        outlineVisibility() &&
        !d->m_isEditingAssistants &&
        kritaProxy->supportsPaintingAssistants();

    Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
        assistant->drawAssistant(gc, updateRect, converter, canvas->displayRendererInterface(), d->useCache, canvas, assistantVisibility(), outlineVisible);

        if (isEditingAssistants()) {
            drawHandles(assistant, gc, converter, canvas->displayRendererInterface());
        }
    }

    if (isEditingAssistants()) {
        Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
            drawEditorWidget(assistant, gc, converter, canvas->displayRendererInterface());
        }
     }
}

void KisPaintingAssistantsDecoration::drawHandles(KisPaintingAssistantSP assistant, QPainter& gc, const KisCoordinatesConverter *converter, const KoColorDisplayRendererInterface *renderInterface)
{
        QTransform initialTransform = converter->documentToWidgetTransform();

    KoColor c;
        c.fromQColor(assistant->effectiveAssistantColor());
        QColor colorToPaint = renderInterface->convertColorToDisplayColorSpace(c);

        Q_FOREACH (const KisPaintingAssistantHandleSP handle, assistant->handles()) {

            QPointF transformedHandle = initialTransform.map(*handle);
            QRectF ellipse(transformedHandle -  QPointF(handleSize() * 0.5, handleSize() * 0.5), QSizeF(handleSize(), handleSize()));

            QPainterPath path;
            path.addEllipse(ellipse);

            gc.save();
            gc.setPen(Qt::NoPen);
            gc.setBrush(colorToPaint);
            gc.drawPath(path);
            gc.restore();
        }

         Q_FOREACH (const KisPaintingAssistantHandleSP handle, assistant->sideHandles()) {
             QPointF transformedHandle = initialTransform.map(*handle);
             QRectF ellipse(transformedHandle -  QPointF(handleSize() * 0.5, handleSize() * 0.5), QSizeF(handleSize(), handleSize()));

             QPainterPath path;
             path.addEllipse(ellipse);

             gc.save();
             gc.setPen(Qt::NoPen);
             gc.setBrush(colorToPaint);
             gc.drawPath(path);
             gc.restore();
         }
}

int KisPaintingAssistantsDecoration::handleSize()
{
    return  d->m_handleSize;
}

void KisPaintingAssistantsDecoration::setHandleSize(int handleSize)
{
    d->m_handleSize = handleSize;
}

QList<KisPaintingAssistantHandleSP> KisPaintingAssistantsDecoration::handles()
{
    QList<KisPaintingAssistantHandleSP> hs;
    Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
        Q_FOREACH (const KisPaintingAssistantHandleSP handle, assistant->handles()) {
            if (!hs.contains(handle)) {
                hs.push_back(handle);
            }
        }
        Q_FOREACH (const KisPaintingAssistantHandleSP handle, assistant->sideHandles()) {
            if (!hs.contains(handle)) {
                hs.push_back(handle);
            }
        }
    }
    return hs;
}

QList<KisPaintingAssistantSP> KisPaintingAssistantsDecoration::assistants() const
{
    QList<KisPaintingAssistantSP> assistants;
    if (view()) {
        if (view()->document()) {
            assistants = view()->document()->assistants();
        }
    }
    return assistants;
}

bool KisPaintingAssistantsDecoration::hasPaintableAssistants() const
{
    return !assistants().isEmpty();
}

KisPaintingAssistantSP KisPaintingAssistantsDecoration::selectedAssistant()
{
    return d->selectedAssistant;
}

void KisPaintingAssistantsDecoration::setSelectedAssistant(KisPaintingAssistantSP assistant)
{
    d->selectedAssistant = assistant;
    Q_EMIT selectedAssistantChanged();
}

void KisPaintingAssistantsDecoration::deselectAssistant()
{
    d->selectedAssistant.clear();
}

void KisPaintingAssistantsDecoration::setAssistantVisible(bool set)
{
    d->assistantVisible=set;
}

void KisPaintingAssistantsDecoration::setOutlineVisible(bool set)
{
    d->outlineVisible=set;
}

void KisPaintingAssistantsDecoration::setOnlyOneAssistantSnap(bool assistant)
{
    d->snapOnlyOneAssistant = assistant;
}

void KisPaintingAssistantsDecoration::setEraserSnap(bool assistant)
{
    d->snapEraser = assistant;
}

bool KisPaintingAssistantsDecoration::assistantVisibility()
{
    return d->assistantVisible;
}
bool KisPaintingAssistantsDecoration::outlineVisibility()
{
    return d->outlineVisible;
}
void KisPaintingAssistantsDecoration::uncache()
{
    Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
        assistant->uncache();
    }

    if (d->m_canvas) {
        d->m_canvas->updateCanvasDecorations();
    }
}
void KisPaintingAssistantsDecoration::toggleAssistantVisible()
{
    setAssistantVisible(!assistantVisibility());
    uncache();
}

void KisPaintingAssistantsDecoration::toggleOutlineVisible()
{
    setOutlineVisible(!outlineVisibility());
    uncache();
}

QColor KisPaintingAssistantsDecoration::globalAssistantsColor()
{
    return view()->document()->assistantsGlobalColor();
}

void KisPaintingAssistantsDecoration::setGlobalAssistantsColor(QColor color)
{
    view()->document()->setAssistantsGlobalColor(color);

    Q_FOREACH (KisPaintingAssistantSP assistant, assistants()) {
        assistant->setAssistantGlobalColorCache(color);
    }

    uncache();
}

void KisPaintingAssistantsDecoration::activateAssistantsEditor()
{
    setVisible(true);
    d->m_isEditingAssistants = true;
    uncache();
}

void KisPaintingAssistantsDecoration::deactivateAssistantsEditor()
{
    if (!d->m_canvas) {
        return;
    }

    d->m_isEditingAssistants = false;
    uncache();
}

bool KisPaintingAssistantsDecoration::isEditingAssistants()
{
    return d->m_isEditingAssistants;
}

QPointF KisPaintingAssistantsDecoration::snapToGuide(KoPointerEvent *e, const QPointF &offset, bool useModifiers)
{
    if (!d->m_canvas || !d->m_canvas->currentImage()) {
        return e->point;
    }

    KoSnapGuide *snapGuide = d->m_canvas->snapGuide();
    QPointF pos = snapGuide->snap(e->point, offset, useModifiers ? e->modifiers() : Qt::NoModifier);

    return pos;
}

QPointF KisPaintingAssistantsDecoration::snapToGuide(const QPointF& pt, const QPointF &offset)
{
    if (!d->m_canvas) {
         return pt;
    }

    KoSnapGuide *snapGuide = d->m_canvas->snapGuide();
    QPointF pos = snapGuide->snap(pt, offset, Qt::NoModifier);

    return pos;
}

void drawSingleState(QPainter& gc, const QIcon &icon, const QSize &iconSize, QRect maxRect, const QPoint &position, const KoColorDisplayRendererInterface *renderInterface, QPoint additional = QPoint()) {
    maxRect.moveTopLeft(position+additional);
    const QMargins m(2, 2, 2, 2);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QRect target(QPoint(), iconSize);
    target.moveCenter(maxRect.center());
    target = target.marginsRemoved(m);
    icon.paint(&gc, target);
#else

    QImage ic = icon.pixmap(iconSize.width(), iconSize.height()).toImage();
    QRect target(QPoint(), (ic.size()/ic.devicePixelRatioF()));
    target.moveCenter(maxRect.center());
    target = target.marginsRemoved(m);
    gc.drawImage(target.intersected(maxRect), renderInterface->convertImageToDisplayColorSpace(ic));
#endif
}

void drawDoubleState(QPainter& gc, const QIcon &icon, const QSize &iconSize, const QIcon &icon2, QRect maxRect, const QPoint &position, const bool state, const KoColorDisplayRendererInterface *renderInterface) {
    maxRect.moveTopLeft(position);
    const QMargins m(2, 2, 2, 2);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QRect target(QPoint(), iconSize);
    target.moveCenter(maxRect.center());
    target = target.marginsRemoved(m);
    if (state) {
        icon.paint(&gc, target);
    } else {
        gc.save();
        gc.setOpacity(0.35);
        icon2.paint(&gc, target);
        gc.restore();
    }
#else
    QImage ic = state? icon.pixmap(iconSize.width(), iconSize.height()).toImage(): icon2.pixmap(iconSize.width(), iconSize.height()).toImage();
    QRect target(QPoint(), (ic.size()/ic.devicePixelRatioF()));
    target.moveCenter(maxRect.center());
    target = target.marginsRemoved(m);
    if (state) {
        gc.drawImage(target.intersected(maxRect), renderInterface->convertImageToDisplayColorSpace(ic));
    } else {
        gc.save();
        gc.setOpacity(0.35);
        gc.drawImage(target.intersected(maxRect.marginsRemoved(m)), renderInterface->convertImageToDisplayColorSpace(ic));
        gc.restore();
    }
#endif
}

void KisPaintingAssistantsDecoration::drawEditorWidget(KisPaintingAssistantSP assistant, QPainter& gc, const KisCoordinatesConverter *converter, const KoColorDisplayRendererInterface *renderInterface)
{
    if (!assistant->isAssistantComplete() || !globalEditorWidgetData.widgetActivated) {
        return;
    }

    QTransform initialTransform = converter->documentToWidgetTransform();
    QPointF actionsPosition = initialTransform.map(assistant->viewportConstrainedEditorPosition(converter, globalEditorWidgetData.boundingSize));

    QPalette sysPalette = renderInterface->systemPaletteForDisplayColorSpace();

    QBrush backgroundColor = sysPalette.window();
    QPointF actionsBGRectangle(actionsPosition + QPointF(globalEditorWidgetData.widgetOffset,globalEditorWidgetData.widgetOffset));
    QPen stroke(sysPalette.window(), 2);

    gc.setRenderHint(QPainter::Antialiasing);

    QPainterPath bgPath;
    bgPath.addRoundedRect(QRectF(actionsBGRectangle.x(), actionsBGRectangle.y(), globalEditorWidgetData.boundingSize.width(), globalEditorWidgetData.boundingSize.height()), 6, 6);

    if (selectedAssistant()) {
        if (assistant->getEditorPosition() == selectedAssistant()->getEditorPosition()) {
            stroke.setWidth(6);
            stroke.setColor(sysPalette.color(QPalette::Highlight));

        }
    }

    gc.setPen(stroke);
    gc.drawPath(bgPath);
    gc.fillPath(bgPath, backgroundColor);

    KoColor c;
    c.fromQColor(QColor(150,150,150,255));
    QColor dragDecorationColor = renderInterface->convertColorToDisplayColorSpace(c);

    QPainterPath dragRect;
    int width = actionsPosition.x()+globalEditorWidgetData.boundingSize.width()-globalEditorWidgetData.dragDecorationWidth+globalEditorWidgetData.widgetOffset;
    dragRect.addRect(QRectF(width,actionsPosition.y()+globalEditorWidgetData.widgetOffset,globalEditorWidgetData.dragDecorationWidth,globalEditorWidgetData.boundingSize.height()));

    gc.fillPath(bgPath.intersected(dragRect),dragDecorationColor);

    QPainterPath dragRectDots;
    c.fromQColor(QColor(50,50,50,255));
    QColor dragDecorationDotsColor = renderInterface->convertColorToDisplayColorSpace(c);
    int dotSize = 2;
    dragRectDots.addEllipse(3,2.5,dotSize,dotSize);
    dragRectDots.addEllipse(3,7.5,dotSize,dotSize);
    dragRectDots.addEllipse(3,-2.5,dotSize,dotSize);
    dragRectDots.addEllipse(3,-7.5,dotSize,dotSize);
    dragRectDots.addEllipse(-3,2.5,dotSize,dotSize);
    dragRectDots.addEllipse(-3,7.5,dotSize,dotSize);
    dragRectDots.addEllipse(-3,-2.5,dotSize,dotSize);
    dragRectDots.addEllipse(-3,-7.5,dotSize,dotSize);
    dragRectDots.translate((globalEditorWidgetData.dragDecorationWidth/2)+width,(globalEditorWidgetData.boundingSize.height()/2)+actionsPosition.y()+globalEditorWidgetData.widgetOffset);
    gc.fillPath(dragRectDots,dragDecorationDotsColor);

    QRect buttonRect(QPoint(0,0), QSize(globalEditorWidgetData.buttonSize, globalEditorWidgetData.buttonSize));
    if (globalEditorWidgetData.moveButtonActivated) {
        QPoint pos = QPointF(actionsPosition + globalEditorWidgetData.moveIconPosition).toPoint();
        drawSingleState(gc, globalEditorWidgetData.m_iconMove, QSize(globalEditorWidgetData.buttonSize+10, globalEditorWidgetData.buttonSize+10), buttonRect, pos, renderInterface, QPoint(5, 5));
    }
    if (globalEditorWidgetData.snapButtonActivated) {
        QPoint pos = QPointF(actionsPosition + globalEditorWidgetData.snapIconPosition).toPoint();
        drawDoubleState(gc, globalEditorWidgetData.m_iconSnapOn, QSize(globalEditorWidgetData.buttonSize, globalEditorWidgetData.buttonSize), globalEditorWidgetData.m_iconSnapOff, buttonRect, pos, assistant->isSnappingActive(), renderInterface);
    }
    if (globalEditorWidgetData.lockButtonActivated) {
        QPoint pos = QPointF(actionsPosition + globalEditorWidgetData.lockedIconPosition).toPoint();
        drawDoubleState(gc, globalEditorWidgetData.m_iconLockOn, QSize(globalEditorWidgetData.buttonSize, globalEditorWidgetData.buttonSize), globalEditorWidgetData.m_iconLockOff, buttonRect, pos, assistant->isLocked(), renderInterface);
    }
    if (globalEditorWidgetData.duplicateButtonActivated) {
        QPoint pos = QPointF(actionsPosition + globalEditorWidgetData.duplicateIconPosition).toPoint();
        drawDoubleState(gc, globalEditorWidgetData.m_iconDuplicate, QSize(globalEditorWidgetData.buttonSize, globalEditorWidgetData.buttonSize), globalEditorWidgetData.m_iconDuplicate, buttonRect, pos, !assistant->isDuplicating(), renderInterface);
    }
    if (globalEditorWidgetData.deleteButtonActivated) {
        QPoint pos = QPointF(actionsPosition + globalEditorWidgetData.deleteIconPosition).toPoint();
        drawSingleState(gc, globalEditorWidgetData.m_iconDelete, QSize(globalEditorWidgetData.buttonSize, globalEditorWidgetData.buttonSize), buttonRect, pos, renderInterface);
    }
}
