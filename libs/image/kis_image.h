/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_IMAGE_H_
#define KIS_IMAGE_H_

#include <QObject>
#include <QString>
#include <QPainter>
#include <QRect>

#include <KoColorConversionTransformation.h>

#include "kis_types.h"
#include "kis_shared.h"
#include "kis_node_graph_listener.h"
#include "kis_node_facade.h"
#include "kis_image_interfaces.h"
#include "kis_strokes_queue_undo_result.h"
#include "KisLodPreferences.h"
#include "KisWraparoundAxis.h"

#include <kritaimage_export.h>

#include <kundo2magicstring.h>

#include <kis_node.h>
class KoColorSpace;
class KoColor;

class KisCompositeProgressProxy;
class KisUndoStore;
class KisUndoAdapter;
class KisImageSignalRouter;
class KisPostExecutionUndoAdapter;
class KisFilterStrategy;
class KoColorProfile;
class KisLayerComposition;
class KisSpontaneousJob;
class KisImageAnimationInterface;
class KUndo2MagicString;
class KisProofingConfiguration;
class KisPaintDevice;
class KisImageGlobalSelectionManagementInterface;

namespace KisMetaData
{
class MergeStrategy;
}

class KRITAIMAGE_EXPORT KisImage : public QObject,
        public KisStrokesFacade,
        public KisStrokeUndoFacade,
        public KisUpdatesFacade,
        public KisProjectionUpdateListener,
        public KisNodeFacade,
        public KisNodeGraphListener,
        public KisShared
{

    Q_OBJECT

public:
    KisImage(KisUndoStore *undoStore, qint32 width, qint32 height, const KoColorSpace *colorSpace, const QString& name);
    ~KisImage() override;

    static KisImageSP fromQImage(const QImage &image, KisUndoStore *undoStore);

public:

    void aboutToAddANode(KisNode *parent, int index) override;
    void nodeHasBeenAdded(KisNode *parent, int index, KisNodeAdditionFlags flags) override;
    void aboutToRemoveANode(KisNode *parent, int index) override;
    void nodeChanged(KisNode * node) override;
    void nodeCollapsedChanged(KisNode *node) override;
    void invalidateAllFrames() override;
    void notifySelectionChanged() override;
    void requestProjectionUpdate(KisNode *node, const QVector<QRect> &rects, KisProjectionUpdateFlags flags) override;
    void invalidateFrames(const KisTimeSpan &range, const QRect &rect) override;
    void requestTimeSwitch(int time) override;
    KisNode* graphOverlayNode() const override;

    void keyframeChannelHasBeenAdded(KisNode *node, KisKeyframeChannel *channel) override;
    void keyframeChannelAboutToBeRemoved(KisNode *node, KisKeyframeChannel *channel) override;

public:
    void notifyProjectionUpdated(const QRect &rc) override;

public:

    void setWorkingThreadsLimit(int value);

    int workingThreadsLimit() const;

    KisImage *clone(bool exactCopy = false);

    void copyFromImage(const KisImage &rhs);

private:

    enum CopyPolicy {
        CONSTRUCT = 1,
        REPLACE = 2,
        EXACT_COPY = 4,
    };

    void copyFromImageImpl(const KisImage &rhs, int policy);

public:

    QImage convertToQImage(qint32 x1,
                           qint32 y1,
                           qint32 width,
                           qint32 height,
                           const KoColorProfile * profile);

    QImage convertToQImage(QRect imageRect,
                           const KoColorProfile * profile);

    QImage convertToQImage(const QSize& scaledImageSize, const KoColorProfile *profile);

    void immediateLockForReadOnly();

    void unlock();

    bool locked() const;

    void setOverlaySelectionMask(KisSelectionMaskSP mask);

    KisSelectionMaskSP overlaySelectionMask() const;

    bool hasOverlaySelectionMask() const;

    KisSelectionSP globalSelection() const;

    QString nextLayerName(const QString &baseName = "") const;

    void resizeImage(const QRect& newRect, const KUndo2MagicString &customActionName = KUndo2MagicString());

    void cropImage(const QRect& newRect);

    void purgeUnusedData(bool isCancellable);

    void cropNode(KisNodeSP node, const QRect& newRect, const bool activeFrameOnly = false);
    void cropNodes(KisNodeList nodes, const QRect& newRect, const bool activeFrameOnly = false);

    void scaleImage(const QSize &size, qreal xres, qreal yres, KisFilterStrategy *filterStrategy);

    void scaleNode(KisNodeSP node, const QPointF &center, qreal scaleX, qreal scaleY, KisFilterStrategy *filterStrategy, KisSelectionSP selection);
    void scaleNodes(KisNodeList nodes, const QPointF &center, qreal scaleX, qreal scaleY, KisFilterStrategy *filterStrategy, KisSelectionSP selection);

    void rotateImage(double radians);

    void rotateNode(KisNodeSP node, double radians, KisSelectionSP selection);
    void rotateNodes(KisNodeList nodes, double radians, KisSelectionSP selection);

    void shear(double angleX, double angleY);

    void shearNode(KisNodeSP node, double angleX, double angleY, KisSelectionSP selection);
    void shearNodes(KisNodeList nodes, double angleX, double angleY, KisSelectionSP selection);

    void convertImageProjectionColorSpace(const KoColorSpace *dstColorSpace);

    void convertImageColorSpace(const KoColorSpace *dstColorSpace,
                                KoColorConversionTransformation::Intent renderingIntent,
                                KoColorConversionTransformation::ConversionFlags conversionFlags);

    void unifyLayersColorSpace();

    void convertLayerColorSpace(KisNodeSP node,
                                const KoColorSpace *dstColorSpace,
                                KoColorConversionTransformation::Intent renderingIntent,
                                KoColorConversionTransformation::ConversionFlags conversionFlags);

    const KoColorProfile *  profile() const;

    bool assignLayerProfile(KisNodeSP node, const KoColorProfile *profile);

    bool assignImageProfile(const KoColorProfile *profile, bool blockAllUpdates = false);

    KisUndoAdapter* undoAdapter() const;

    KisPostExecutionUndoAdapter* postExecutionUndoAdapter() const override;

    const KUndo2Command* lastExecutedCommand() const override;

    void setUndoStore(KisUndoStore *undoStore);

    KisUndoStore* undoStore();

    void setModifiedWithoutUndo();

    const KoColorSpace * colorSpace() const;

    double xRes() const;

    double yRes() const;

    void setResolution(double xres, double yres);

    QPointF documentToPixel(const QPointF &documentCoord) const;

    QPoint documentToImagePixelFloored(const QPointF &documentCoord) const;

    QRectF documentToPixel(const QRectF &documentRect) const;

    QPointF pixelToDocument(const QPointF &pixelCoord) const;

    QPointF pixelToDocument(const QPoint &pixelCoord) const;

    QRectF pixelToDocument(const QRectF &pixelCoord) const;

    qint32 width() const;

    qint32 height() const;

    QSize size() const {
        return QSize(width(), height());
    }

    KisGroupLayerSP rootLayer() const;

    KisPaintDeviceSP projection() const;

    qint32 nlayers() const;

    qint32 nHiddenLayers() const;

    qint32 nChildLayers() const;

    void flatten(KisNodeSP activeNode);

    void mergeDown(KisLayerSP l, const KisMetaData::MergeStrategy* strategy);

    void flattenLayer(KisLayerSP layer);

    void mergeMultipleLayers(QList<KisNodeSP> mergedLayers, KisNodeSP putAfter);

    QRect bounds() const override;

    QRect effectiveLodBounds() const;

    void notifyLayersChanged();

    void setDefaultProjectionColor(const KoColor &color);

    KoColor defaultProjectionColor() const;

    void setRootLayer(KisGroupLayerSP rootLayer);

    void addAnnotation(KisAnnotationSP annotation);

    KisAnnotationSP annotation(const QString& type);

    void removeAnnotation(const QString& type);

    vKisAnnotationSP_it beginAnnotations();

    vKisAnnotationSP_it endAnnotations();

    void notifyAboutToBeDeleted();

    KisImageSignalRouter* signalRouter();

    bool canReselectGlobalSelection();

    QList<KisLayerCompositionSP> compositions();

    void addComposition(KisLayerCompositionSP composition);

    void removeComposition(KisLayerCompositionSP composition);

    void moveCompositionUp(KisLayerCompositionSP composition);

    void moveCompositionDown(KisLayerCompositionSP composition);

    void setWrapAroundModePermitted(bool value);

    bool wrapAroundModePermitted() const;

    void setWrapAroundModeAxis(WrapAroundAxis value);
    WrapAroundAxis wrapAroundModeAxis() const;

    bool wrapAroundModeActive() const;

    int currentLevelOfDetail() const;

    QPointF mirrorAxesCenter() const;

    void setMirrorAxesCenter(const QPointF &value) const;

    void setAllowMasksOnRootNode(bool value);

    bool allowMasksOnRootNode() const;

public Q_SLOTS:

    void explicitRegenerateLevelOfDetail();

public:

    void setLodPreferences(const KisLodPreferences &value);

    KisLodPreferences lodPreferences() const;

    KisImageAnimationInterface *animationInterface() const;

    void setProofingConfiguration(KisProofingConfigurationSP proofingConfig);
    KisProofingConfigurationSP proofingConfiguration() const;

    bool startIsolatedMode(KisNodeSP node, bool isolateLayer, bool isolateGroup);

public Q_SLOTS:
    void stopIsolatedMode();

public:
    KisNodeSP isolationRootNode() const;
    bool isIsolatingLayer() const;
    bool isIsolatingGroup() const;

Q_SIGNALS:

    void sigImageUpdated(const QRect &);

    void sigImageModified();

    void sigImageModifiedWithoutUndo();

    void sigSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint);

    void sigProfileChanged(const KoColorProfile *  profile);
    void sigColorSpaceChanged(const KoColorSpace*  cs);
    void sigResolutionChanged(double xRes, double yRes);
    void sigRequestNodeReselection(KisNodeSP activeNode, const KisNodeList &selectedNodes);

    void sigNodeChanged(KisNodeSP node);

    void sigAboutToBeDeleted();

    void sigNodeAddedAsync(KisNodeSP node, KisNodeAdditionFlags flags);

    void sigRemoveNodeAsync(KisNodeSP node);

    void sigLayersChangedAsync();

    void sigUndoDuringStrokeRequested();

    void sigRedoDuringStrokeRequested();

    void sigStrokeCancellationRequested();

    void sigStrokeEndRequested();

    void sigStrokeEndRequestedActiveNodeFiltered();

    void sigIsolatedModeChanged();

    void sigNodeCollapsedChanged();

    void sigProofingConfigChanged();

    void sigInternalStopIsolatedModeRequested();

public:
    KisCompositeProgressProxy* compositeProgressProxy();

    bool isIdle(bool allowLocked = false);

    void barrierLock(bool readOnly = false);

    bool tryBarrierLock(bool readOnly = false);

    void waitForDone();

    KisStrokeId startStroke(KisStrokeStrategy *strokeStrategy) override;
    void addJob(KisStrokeId id, KisStrokeJobData *data) override;
    void endStroke(KisStrokeId id) override;
    bool cancelStroke(KisStrokeId id) override;

    void blockUpdates() override;

    void unblockUpdates() override;

    void disableUIUpdates() override;

    void notifyBatchUpdateStarted() override;

    void notifyBatchUpdateEnded() override;

    void notifyUIUpdateCompleted(const QRect &rc) override;

    QVector<QRect> enableUIUpdates() override;

    void disableDirtyRequests() override;

    void enableDirtyRequests() override;

    KisProjectionUpdatesFilterCookie addProjectionUpdatesFilter(KisProjectionUpdatesFilterSP filter) override;

    KisProjectionUpdatesFilterSP removeProjectionUpdatesFilter(KisProjectionUpdatesFilterCookie cookie) override;

    KisProjectionUpdatesFilterCookie currentProjectionUpdatesFilter() const override;

    using KisUpdatesFacade::refreshGraphAsync;
    void refreshGraphAsync(KisNodeSP root, const QVector<QRect> &rects, const QRect &cropRect, KisProjectionUpdateFlags flags = KisProjectionUpdateFlag::None) override;

    void initialRefreshGraph();

    void addSpontaneousJob(KisSpontaneousJob *spontaneousJob);

    bool hasUpdatesRunning() const override;

public Q_SLOTS:

    void requestUndoDuringStroke();

    void requestRedoDuringStroke();

    void requestStrokeCancellation();

    UndoResult tryUndoUnfinishedLod0Stroke();

    void requestStrokeEnd();

    void requestStrokeEndActiveNode();

    KisImageGlobalSelectionManagementInterface* globalSelectionManagementInterface() const;

private:

    KisImage(const KisImage& rhs, KisUndoStore *undoStore, bool exactCopy);
    KisImage& operator=(const KisImage& rhs);

    void emitSizeChanged();

    void resizeImageImpl(const QRect& newRect, bool cropLayers, const KUndo2MagicString &customActionName = KUndo2MagicString());
    void rotateImpl(const KUndo2MagicString &actionName, KisNodeSP rootNode, double radians,
                    bool resizeImage, KisSelectionSP selection);
    void rotateImpl(const KUndo2MagicString &actionName, KisNodeList nodes, double radians,
                    bool resizeImage, KisSelectionSP selection);
    void shearImpl(const KUndo2MagicString &actionName, KisNodeSP rootNode,
                   bool resizeImage, double angleX, double angleY, KisSelectionSP selection);
    void shearImpl(const KUndo2MagicString &actionName, KisNodeList nodes,
                   bool resizeImage, double angleX, double angleY, KisSelectionSP selection);

    void safeRemoveTwoNodes(KisNodeSP node1, KisNodeSP node2);

    void refreshHiddenArea(KisNodeSP rootNode, const QRect &preparedArea);

    friend class KisImageResizeCommand;
    void setSize(const QSize& size);

    void setProjectionColorSpace(const KoColorSpace * colorSpace);

    friend class KisImageGlobalSelectionManagementInterface;

private:
    class KisImagePrivate;
    KisImagePrivate * m_d;
};

#endif
