/*
 * SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KisReferenceImagesDecoration.h"

#include "KisDisplayConfig.h"
#include "KoShapeManager.h"

#include "kis_algebra_2d.h"
#include "KisDocument.h"
#include "KisReferenceImagesLayer.h"
#include "kis_layer_utils.h"
#include <kis_display_color_converter.h>
#include <qcolorspace.h>

struct KisReferenceImagesDecoration::Private {
    struct Buffer
    {
        QPointF position;
        QImage image;

        QRectF bounds() const
        {
            return QRectF(position, image.size() / image.devicePixelRatio());
        }
    };

    KisReferenceImagesDecoration *q;

    KisWeakSharedPtr<KisReferenceImagesLayer> layer;
    Buffer buffer;
    QTransform previousTransform;
    QSizeF previousViewSize;

    explicit Private(KisReferenceImagesDecoration *q)
        : q(q)
    {}

    void updateBufferByImageCoordinates(const QRectF &dirtyImageRect)
    {
        QRectF dirtyWidgetRect = q->view()->viewConverter()->imageToWidget(dirtyImageRect);
        updateBuffer(dirtyWidgetRect, dirtyImageRect);
    }

    void updateBufferByWidgetCoordinates(const QRectF &dirtyWidgetRect)
    {
        QRectF dirtyImageRect = q->view()->viewConverter()->widgetToImage(dirtyWidgetRect);
        updateBuffer(dirtyWidgetRect, dirtyImageRect);
    }

private:
    void updateBuffer(QRectF widgetRect, QRectF imageRect)
    {
        KisCoordinatesConverter *viewConverter = q->view()->viewConverter();
        QTransform transform = viewConverter->imageToWidgetTransform();

        qreal devicePixelRatioF = q->view()->devicePixelRatioF();
        if (buffer.image.isNull() || !buffer.bounds().contains(widgetRect)) {
            const QRectF boundingImageRect = layer->boundingImageRect();
            const QRectF boundingWidgetRect = q->view()->viewConverter()->imageToWidget(boundingImageRect);
            widgetRect = boundingWidgetRect.intersected(q->view()->rect());

            if (widgetRect.isNull()) return;

            buffer.position = widgetRect.topLeft();
            QColorSpace space = buffer.image.colorSpace();
            buffer.image = QImage((widgetRect.size()*devicePixelRatioF).toSize(), QImage::Format_ARGB32);
            buffer.image.setColorSpace(space);
            buffer.image.setDevicePixelRatio(devicePixelRatioF);

            imageRect = q->view()->viewConverter()->widgetToImage(widgetRect);

        }

        QPainter gc(&buffer.image);

        gc.translate(-buffer.position);
        gc.setTransform(transform, true);

        gc.save();
        gc.setCompositionMode(QPainter::CompositionMode_Source);
        gc.fillRect(imageRect, Qt::transparent);
        gc.restore();

        gc.setClipRect(QRectF(imageRect.topLeft(), imageRect.size()*devicePixelRatioF));
        layer->paintReferences(gc);
    }
};

KisReferenceImagesDecoration::KisReferenceImagesDecoration(QPointer<KisView> parent, KisDocument *document, bool viewReady)
    : KisCanvasDecoration("referenceImagesDecoration", parent)
    , d(new Private(this))
{
    connect(document->image().data(), SIGNAL(sigNodeAddedAsync(KisNodeSP, KisNodeAdditionFlags)), this, SLOT(slotNodeAdded(KisNodeSP, KisNodeAdditionFlags)));
    connect(document->image().data(), SIGNAL(sigRemoveNodeAsync(KisNodeSP)), this, SLOT(slotNodeRemoved(KisNodeSP)));
    connect(document->image().data(), SIGNAL(sigLayersChangedAsync()), this, SLOT(slotLayersChanged()));
    connect(document, &KisDocument::sigReferenceImagesLayerChanged, this, qOverload<KisNodeSP>(&KisReferenceImagesDecoration::slotNodeAdded));

    auto referenceImageLayer = document->referenceImagesLayer();
    if (referenceImageLayer) {
        setReferenceImageLayer(referenceImageLayer, viewReady);
    }
}

KisReferenceImagesDecoration::~KisReferenceImagesDecoration()
{}

void KisReferenceImagesDecoration::addReferenceImage(KisReferenceImage *referenceImage)
{
    KUndo2Command *cmd = KisReferenceImagesLayer::addReferenceImages(view()->document(), {referenceImage});
    view()->canvasBase()->addCommand(cmd);
}

bool KisReferenceImagesDecoration::documentHasReferenceImages() const
{
    return view()->document()->referenceImagesLayer() != nullptr;
}

void KisReferenceImagesDecoration::drawDecoration(QPainter &gc, const QRectF & , const KisCoordinatesConverter *converter, KisCanvas2 *canvas)
{

    KisSharedPtr<KisReferenceImagesLayer> layer = d->layer.toStrongRef();
    const QColorSpace canvasSpace = KoColorSpaceRegistry::instance()->QColorSpaceForProfile(canvas->displayColorConverter()->multiSurfaceDisplayConfig().canvasProfile);

    if (!layer.isNull()) {
        QSizeF viewSize = view()->size();

        QTransform transform = converter->imageToWidgetTransform();
        if (d->previousViewSize != viewSize || !KisAlgebra2D::fuzzyMatrixCompare(transform, d->previousTransform, 1e-4)
            || d->buffer.image.colorSpace() != canvasSpace) {
            d->previousViewSize = viewSize;
            d->previousTransform = transform;

            d->buffer.image = QImage(QSize(1, 1), QImage::Format_ARGB32);
            d->buffer.image.setColorSpace(canvasSpace);

            d->updateBufferByWidgetCoordinates(QRectF(QPointF(0,0), viewSize));
        }

        if (!d->buffer.image.isNull()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            gc.drawImage(d->buffer.position, d->buffer.image);
#else
            gc.drawImage(d->buffer.position, d->buffer.image);
#endif
        }
    }
}

void KisReferenceImagesDecoration::slotNodeAdded(KisNodeSP node)
{
    slotNodeAdded(node, KisNodeAdditionFlag::None);
}

void KisReferenceImagesDecoration::slotNodeAdded(KisNodeSP node, KisNodeAdditionFlags flags)
{
    Q_UNUSED(flags)

    KisReferenceImagesLayer *referenceImagesLayer =
        dynamic_cast<KisReferenceImagesLayer*>(node.data());

    if (referenceImagesLayer) {
        setReferenceImageLayer(referenceImagesLayer, true);
    }
}

void KisReferenceImagesDecoration::slotNodeRemoved(KisNodeSP node)
{
    KisReferenceImagesLayer *referenceImagesLayer =
        dynamic_cast<KisReferenceImagesLayer*>(node.data());

    if (referenceImagesLayer && referenceImagesLayer == d->layer) {
        setReferenceImageLayer(0, true);
    }
}

void KisReferenceImagesDecoration::slotLayersChanged()
{
    KisImageSP image = view()->image();

    KisReferenceImagesLayer *referenceImagesLayer =
        KisLayerUtils::findNodeByType<KisReferenceImagesLayer>(image->root());

    setReferenceImageLayer(referenceImagesLayer, true);
}

void KisReferenceImagesDecoration::slotReferenceImagesChanged(const QRectF &dirtyRect)
{
    d->updateBufferByImageCoordinates(dirtyRect);

    QRectF documentRect = view()->viewConverter()->imageToDocument(dirtyRect);
    view()->canvasBase()->updateCanvasDecorations(documentRect);
}

void KisReferenceImagesDecoration::setReferenceImageLayer(KisSharedPtr<KisReferenceImagesLayer> layer, bool updateCanvas)
{
    if (d->layer != layer.data()) {
        KisSharedPtr<KisReferenceImagesLayer> oldLayer = d->layer.toStrongRef();
        if (oldLayer) {
            oldLayer->disconnect(this);
        }

        d->layer = layer;

        if (layer) {
            connect(layer.data(), SIGNAL(sigUpdateCanvas(QRectF)),
                    this, SLOT(slotReferenceImagesChanged(QRectF)));

            const QRectF dirtyRect = layer->boundingImageRect();

            if (updateCanvas && !dirtyRect.isEmpty()) {
                slotReferenceImagesChanged(dirtyRect);
            }
        }
    }
}
