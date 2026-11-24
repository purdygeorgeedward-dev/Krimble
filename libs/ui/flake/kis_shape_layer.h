/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SHAPE_LAYER_H_
#define KIS_SHAPE_LAYER_H_

#include <KoShapeLayer.h>

#include <kis_types.h>
#include <kis_external_layer_iface.h>
#include <kritaui_export.h>
#include <KisDelayedUpdateNodeInterface.h>
#include <KisCroppedOriginalLayerInterface.h>

class QRect;
class QIcon;
class QRect;
class QString;
class KoShapeManager;
class KoStore;
class KoViewConverter;
class KoShapeControllerBase;
class KoDocumentResourceManager;
class KisShapeLayerCanvasBase;
class KoSelectedShapesProxy;

const QString KIS_SHAPE_LAYER_ID = "KisShapeLayer";
class KRITAUI_EXPORT KisShapeLayer
        : public KisExternalLayer,
        public KoShapeLayer,
        public KisDelayedUpdateNodeInterface,
        public KisCroppedOriginalLayerInterface
{
    Q_OBJECT

public:

    KisShapeLayer(KoShapeControllerBase* shapeController, KisImageWSP image, const QString &name, quint8 opacity);
    KisShapeLayer(const KisShapeLayer& _rhs);
    KisShapeLayer(const KisShapeLayer& _rhs, KoShapeControllerBase* controller);
    KisShapeLayer(const KisShapeLayer& _rhs, KoShapeControllerBase* controller, std::function<KisShapeLayerCanvasBase*()> canvasFactory);

    KisShapeLayer(const KisShapeLayer& baseTemplate, const QList<KoShape*> &newShapes);

    ~KisShapeLayer() override;

    KisBaseNode::PropertyList sectionModelProperties() const override;
    void setSectionModelProperties(const KisBaseNode::PropertyList &properties) override;

protected:
    KisShapeLayer(KoShapeControllerBase* shapeController, KisImageWSP image, const QString &name, quint8 opacity, std::function<KisShapeLayerCanvasBase *()> canvasFactory);
private:
    void initShapeLayerImpl(KoShapeControllerBase* controller, KisShapeLayerCanvasBase *overrideCanvas);
public:
    KisNodeSP clone() const override {
        return new KisShapeLayer(*this);
    }
    bool allowAsChild(KisNodeSP) const override;

    void setImage(KisImageWSP image) override;

    KisLayerSP tryCreateInternallyMergedLayerFromMutipleLayers(QList<KisLayerSP> layers) override;

    KisLayerSP createMergedLayerTemplate(KisLayerSP prevLayer) override;
    void fillMergedLayerTemplate(KisLayerSP dstLayer, KisLayerSP prevLayer, bool skipPaintingThisLayer) override;
public:

    bool isSelectable() const {
        return false;
    }

    void setParent(KoShapeContainer *parent);

    QIcon icon() const override;
    void resetCache(const KoColorSpace *colorSpace) override;

    KisPaintDeviceSP original() const override;
    KisPaintDeviceSP paintDevice() const override;

    QRect theoreticalBoundingRect() const override;

    qint32 x() const override;
    qint32 y() const override;
    void setX(qint32) override;
    void setY(qint32) override;

    bool accept(KisNodeVisitor&) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    KoShapeManager *shapeManager() const;

    static bool saveShapesToStore(KoStore *store, QList<KoShape*> shapes, const QSizeF &sizeInPt);

    static QList<KoShape *> createShapesFromSvg(QIODevice *device,
                                                const QString &baseXmlDir,
                                                const QRectF &rectInPixels,
                                                qreal resolutionPPI,
                                                KoDocumentResourceManager *resourceManager,
                                                bool loadingFromKra,
                                                QSizeF *fragmentSize,
                                                QStringList *warnings = 0,
                                                QStringList *errors = 0);

    bool saveLayer(KoStore * store) const;
    bool loadLayer(KoStore* store, QStringList *warnings = 0);

    KUndo2Command* crop(const QRect & rect) override;
    KUndo2Command* transform(const QTransform &transform) override;
    KUndo2Command* setProfile(const KoColorProfile *profile) override;
    KUndo2Command* convertTo(const KoColorSpace * dstColorSpace,
                                 KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                                 KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags()) override;

    bool visible(bool recursive = false) const override;
    void setVisible(bool visible, bool isLoading = false) override;

    void setUserLocked(bool value) override;

    bool isShapeEditable(bool recursive) const override;

    void forceUpdateTimedNode() override;

    bool hasPendingTimedUpdates() const override;

    void forceUpdateHiddenAreaOnOriginal() override;

    KoSelectedShapesProxy* selectedShapesProxy();

    bool antialiased() const;
    void setAntialiased(const bool antialiased);

protected:
    using KoShape::isVisible;

    bool loadSvg(QIODevice *device, const QString &baseXmlDir, QStringList *warnings = 0);

    friend class ShapeLayerContainerModel;
    const KoViewConverter *converter() const;

    KoShapeControllerBase *shapeController() const;
    KisShapeLayerCanvasBase *canvas() const;

    friend class TransformShapeLayerDeferred;

Q_SIGNALS:
    void selectionChanged();
    void currentLayerChanged(const KoShapeLayer *layer);

Q_SIGNALS:
    void sigMoveShapes(const QPointF &diff);

private Q_SLOTS:
    void slotMoveShapes(const QPointF &diff);
    void slotTransformShapes(const QTransform &transform);
    void slotImageResolutionChanged();

private:
    QList<KoShape*> shapesToBeTransformed();

private:
    struct Private;
    Private * const m_d;
};

#endif
