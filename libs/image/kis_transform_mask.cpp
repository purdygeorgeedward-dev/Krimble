/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QMutex>
#include <QMutexLocker>
#include <KoIcon.h>
#include <kis_icon.h>
#include <KoCompositeOpRegistry.h>

#include "kis_layer.h"
#include "kis_transform_mask.h"
#include <kis_group_layer.h>
#include "filter/kis_filter.h"
#include "filter/kis_filter_configuration.h"
#include "filter/kis_filter_registry.h"
#include "kis_selection.h"
#include "kis_processing_information.h"
#include "kis_node.h"
#include "kis_node_visitor.h"
#include "kis_processing_visitor.h"
#include "kis_node_progress_proxy.h"
#include "kis_painter.h"

#include "kis_busy_progress_indicator.h"
#include "kis_perspectivetransform_worker.h"
#include "kis_transform_mask_params_interface.h"
#include "kis_transform_mask_params_factory_registry.h"
#include "kis_recalculate_transform_mask_job.h"
#include "kis_thread_safe_signal_compressor.h"
#include "kis_algebra_2d.h"
#include "kis_safe_transform.h"
#include "kis_keyframe_channel.h"
#include "kis_raster_keyframe_channel.h"
#include "kis_scalar_keyframe_channel.h"

#include "kis_image_config.h"
#include "kis_lod_capable_layer_offset.h"

#include <QReadWriteLock>
#include "KisTransformMaskTestingInterface.h"

namespace {

class StaticCacheStorage
{
public:
    bool isCacheValid(KisTransformMaskParamsInterfaceSP currentParams) const {
        QReadLocker l(&m_lock);

        KIS_SAFE_ASSERT_RECOVER_NOOP(!staticCacheValid ||
                                     paramsForStaticImage ||
                                     staticCacheIsOverridden);

        return staticCacheValid &&
            (!paramsForStaticImage ||
             paramsForStaticImage->compareTransform(currentParams));
    }

    bool isCacheOverridden() const {
        QReadLocker l(&m_lock);

        KIS_SAFE_ASSERT_RECOVER_NOOP(!staticCacheIsOverridden || staticCacheValid);

        return staticCacheIsOverridden;
    }

    void lazyAllocateStaticCache(const KoColorSpace *cs, KisDefaultBoundsBaseSP defaultBounds) {
        QWriteLocker l(&m_lock);

        if (!staticCacheDevice ||
            *staticCacheDevice->colorSpace() != *cs) {

            staticCacheDevice = new KisPaintDevice(cs);
            staticCacheDevice->setDefaultBounds(defaultBounds);
        }
    }

    KisPaintDeviceSP device() {
        return staticCacheDevice;
    }

    void setDeviceCacheValid(KisTransformMaskParamsInterfaceSP currentParams) {
        QWriteLocker l(&m_lock);

        paramsForStaticImage = currentParams;
        staticCacheValid = true;
        KIS_SAFE_ASSERT_RECOVER_NOOP(!staticCacheIsOverridden);
    }

    void overrideStaticCacheDevice(KisPaintDeviceSP device)
    {
        KIS_SAFE_ASSERT_RECOVER_RETURN(staticCacheDevice);

        staticCacheDevice->clear();

        if (device) {
            const QRect rc = device->extent();
            KisPainter::copyAreaOptimized(rc.topLeft(), device, staticCacheDevice, rc);
        }

        {
            QWriteLocker l(&m_lock);
            paramsForStaticImage.clear();
            staticCacheValid = bool(device);
            staticCacheIsOverridden = bool(device);
        }
    }

    void invalidateDeviceCache() {
        staticCacheValid = false;
        paramsForStaticImage.clear();
        KIS_SAFE_ASSERT_RECOVER_NOOP(!staticCacheIsOverridden);
    }

private:
    mutable QReadWriteLock m_lock;
    bool staticCacheIsOverridden {false};
    bool staticCacheValid = {false};
    KisPaintDeviceSP staticCacheDevice;
    KisTransformMaskParamsInterfaceSP paramsForStaticImage;
};

struct AccumulatedRectStorage
{
    void addRect(const QRect &rc) {
        QMutexLocker l(&m_mutex);
        m_rect |= rc;
    }

    QRect takeRect() {
        QMutexLocker l(&m_mutex);

        const QRect rect = m_rect;
        m_rect = QRect();

        return rect;
    }

private:
    QMutex m_mutex;
    QRect m_rect;
};

}

#define UPDATE_DELAY 3000

struct Q_DECL_HIDDEN KisTransformMask::Private
{
    Private(KisImageSP image)
        : worker(0, QTransform(), true, 0),
          paramsHolder(KisTransformMaskParamsFactoryRegistry::instance()->createAnimatedParamsHolder(new KisDefaultBounds(image))),
          recalculatingStaticImage(false),
          offset(new KisDefaultBounds(image)),
          updateSignalCompressor(UPDATE_DELAY, KisSignalCompressor::POSTPONE),
          offBoundsReadArea(0.5)
    {
    }

    Private(const Private &rhs)
        : worker(rhs.worker),
          paramsHolder(rhs.paramsHolder->clone()),
          recalculatingStaticImage(rhs.recalculatingStaticImage),
          offset(rhs.offset),
          updateSignalCompressor(UPDATE_DELAY, KisSignalCompressor::POSTPONE),
          offBoundsReadArea(rhs.offBoundsReadArea)
    {
    }

    KisPerspectiveTransformWorker worker;
    KisAnimatedTransformParamsHolderInterfaceSP paramsHolder;

    StaticCacheStorage staticCache;
    bool recalculatingStaticImage;

    AccumulatedRectStorage forcedStaticUpdateExtraUpdateRect;

    KisLodCapableLayerOffset offset;

    KisThreadSafeSignalCompressor updateSignalCompressor;
    qreal offBoundsReadArea;

    QScopedPointer<KisTransformMaskTestingInterface> testingInterface;
};

KisTransformMask::KisTransformMask(KisImageWSP image, const QString &name)
    : KisEffectMask(image, name),
      m_d(new Private(image))
{
    connect(&m_d->updateSignalCompressor, SIGNAL(timeout()), SLOT(slotDelayedStaticUpdate()));
    connect(this, SIGNAL(sigInternalForceStaticImageUpdate()), SLOT(slotInternalForceStaticImageUpdate()));
    m_d->offBoundsReadArea = KisImageConfig(true).transformMaskOffBoundsReadArea();
    setSupportsLodMoves(false);
}

KisTransformMask::~KisTransformMask()
{
}

KisTransformMask::KisTransformMask(const KisTransformMask& rhs)
    : KisEffectMask(rhs),
      m_d(new Private(*rhs.m_d))
{
    connect(&m_d->updateSignalCompressor, SIGNAL(timeout()), SLOT(slotDelayedStaticUpdate()));

    const QVector<QString> ids = {KisKeyframeChannel::PositionX.id(),
                                  KisKeyframeChannel::PositionY.id(),
                                  KisKeyframeChannel::ScaleX.id(),
                                  KisKeyframeChannel::ScaleY.id(),
                                  KisKeyframeChannel::ShearX.id(),
                                  KisKeyframeChannel::ShearY.id(),
                                  KisKeyframeChannel::RotationX.id(),
                                  KisKeyframeChannel::RotationY.id(),
                                  KisKeyframeChannel::RotationZ.id()};

    Q_FOREACH (const QString &id, ids) {
        KisKeyframeChannel *channel = m_d->paramsHolder->getKeyframeChannel(id);
        if (channel) {
            addKeyframeChannel(channel);
        }
    }
}

KisPaintDeviceSP KisTransformMask::paintDevice() const
{
    return 0;
}

QIcon KisTransformMask::icon() const
{
    return KisIconUtils::loadIcon("transformMask");
}

void KisTransformMask::setTransformParamsWithUndo(KisTransformMaskParamsInterfaceSP params, KUndo2Command *parentCommand)
{
    KIS_SAFE_ASSERT_RECOVER_RETURN(params);

    m_d->paramsHolder->setParamsAtCurrentPosition(params.data(), parentCommand);
}

void KisTransformMask::setTransformParams(KisTransformMaskParamsInterfaceSP params)
{
    KUndo2Command todo_REMOVE;
    setTransformParamsWithUndo(params, &todo_REMOVE);
    todo_REMOVE.redo();

    m_d->staticCache.invalidateDeviceCache();
    m_d->updateSignalCompressor.start();
}

KisTransformMaskParamsInterfaceSP KisTransformMask::transformParams() const
{
    return m_d->paramsHolder->bakeIntoParams();
}

void KisTransformMask::slotDelayedStaticUpdate()
{
    if (m_d->testingInterface) {
        m_d->testingInterface->notifySlotDelayedStaticUpdate();
    }

    startAsyncRegenerationJob();
}

void KisTransformMask::forceStartAsyncRegenerationJob()
{
    m_d->staticCache.invalidateDeviceCache();
    m_d->updateSignalCompressor.stop();
    startAsyncRegenerationJob();
}

void KisTransformMask::startAsyncRegenerationJob()
{

    KisLayerSP parentLayer = qobject_cast<KisLayer*>(parent().data());
    if (!parentLayer) return;

    KisImageSP image = parentLayer->image();

    if (!image) {
        return;
    }

    if (image->locked()) {
        m_d->updateSignalCompressor.start();
        return;
    }

    const QRect extraUpdateRect = m_d->forcedStaticUpdateExtraUpdateRect.takeRect();
    image->addSpontaneousJob(new KisRecalculateTransformMaskJob(this, extraUpdateRect));
}

KisPaintDeviceSP KisTransformMask::buildPreviewDevice()
{

    KisLayerSP parentLayer = qobject_cast<KisLayer*>(parent().data());
    KIS_ASSERT_RECOVER(parentLayer) { return new KisPaintDevice(colorSpace()); }

    KisPaintDeviceSP device =
        new KisPaintDevice(parentLayer->original()->colorSpace());
    device->setDefaultBounds(parentLayer->original()->defaultBounds());

    QRect requestedRect = parentLayer->original()->exactBounds();
    parentLayer->buildProjectionUpToNode(device, this, requestedRect);

    return device;
}

KisPaintDeviceSP KisTransformMask::buildSourcePreviewDevice()
{

    KisLayerSP parentLayer = qobject_cast<KisLayer*>(parent().data());
    KIS_ASSERT_RECOVER(parentLayer) { return new KisPaintDevice(colorSpace()); }

    KisPaintDeviceSP device =
        new KisPaintDevice(parentLayer->original()->colorSpace());
    device->setDefaultBounds(parentLayer->original()->defaultBounds());

    QRect requestedRect = parentLayer->original()->exactBounds();

    KisNodeSP prevSibling = this->prevSibling();
    if (prevSibling) {
        parentLayer->buildProjectionUpToNode(device, prevSibling, requestedRect);
    } else {
        requestedRect = parentLayer->outgoingChangeRect(requestedRect);
        parentLayer->copyOriginalToProjection(parentLayer->original(), device, requestedRect);
    }

    return device;
}

void KisTransformMask::overrideStaticCacheDevice(KisPaintDeviceSP device)
{
    m_d->staticCache.overrideStaticCacheDevice(device);
}

bool KisTransformMask::staticImageCacheIsValid() const
{
    return m_d->staticCache.isCacheValid(m_d->paramsHolder->bakeIntoParams());
}

void KisTransformMask::recalculateStaticImage()
{
    if (m_d->testingInterface) {
        m_d->testingInterface->notifyRecalculateStaticImage();
    }

    KisLayerSP parentLayer = qobject_cast<KisLayer*>(parent().data());
    KIS_SAFE_ASSERT_RECOVER_RETURN(parentLayer);

    KIS_SAFE_ASSERT_RECOVER_RETURN(parentLayer->projection() != parentLayer->paintDevice());

    m_d->staticCache.lazyAllocateStaticCache(parentLayer->original()->colorSpace(),
                                             parentLayer->original()->defaultBounds());

    m_d->recalculatingStaticImage = true;
    QRect requestedRect =
        parentLayer->changeRect(parentLayer->exactBounds());

    const QRect bounds = parentLayer->original()->defaultBounds()->bounds();
    const QRect limitingRect = KisAlgebra2D::blowRect(bounds, m_d->offBoundsReadArea);

    requestedRect &= limitingRect;

    parentLayer->projection()->clear();

    parentLayer->updateProjection(requestedRect, this, KisRenderPassFlag::NoTransformMaskUpdates);
    m_d->recalculatingStaticImage = false;
}

QRect KisTransformMask::decorateRect(KisPaintDeviceSP &src,
                                     KisPaintDeviceSP &dst,
                                     const QRect & rc,
                                     PositionToFilthy maskPos,
                                     KisRenderPassFlags flags) const
{
    Q_ASSERT_X(src != dst, "KisTransformMask::decorateRect",
               "src must be != dst, because we can't create transactions "
               "during merge, as it breaks reentrancy");

    KisTransformMaskParamsInterfaceSP params = m_d->paramsHolder->bakeIntoParams();

    if (params->isHidden()) return rc;
    KIS_ASSERT_RECOVER_NOOP(maskPos == N_FILTHY ||
                            maskPos == N_ABOVE_FILTHY ||
                            maskPos == N_BELOW_FILTHY);

    if (m_d->paramsHolder->defaultBounds()->externalFrameActive()) {

        if (params->isAffine()) {
            m_d->worker.setForceSubPixelTranslation(m_d->paramsHolder->isAnimated());
            m_d->worker.setForwardTransform(params->finalAffineTransform());
            m_d->worker.runPartialDst(src, dst, rc);

#ifdef DEBUG_RENDERING
            qDebug() << "Partial for external frame" << name() << ppVar(src->exactBounds()) << ppVar(src->extent()) << ppVar(dst->exactBounds()) << ppVar(dst->extent()) << ppVar(rc);
            KIS_DUMP_DEVICE_2(src, DUMP_RECT, "partial_ext_src", "dd");
            KIS_DUMP_DEVICE_2(dst, DUMP_RECT, "partial_ext_dst", "dd");
#endif
        }

        return rc;
    }

    if (!m_d->staticCache.isCacheOverridden() &&
        !m_d->recalculatingStaticImage &&
        (maskPos == N_FILTHY || maskPos == N_ABOVE_FILTHY ||
         !m_d->staticCache.isCacheValid(params)) &&

        !flags.testFlag(KisRenderPassFlag::NoTransformMaskUpdates)) {

        if (m_d->testingInterface) {
            m_d->testingInterface->notifyDecorateRectTriggeredStaticImageUpdate();
        }

        m_d->staticCache.invalidateDeviceCache();
        m_d->updateSignalCompressor.start();
    }

    if (m_d->recalculatingStaticImage) {
        KIS_SAFE_ASSERT_RECOVER_NOOP(!m_d->staticCache.isCacheValid(params));

        KisPaintDeviceSP staticCacheDevice = m_d->staticCache.device();

        staticCacheDevice->clear();
        params->transformDevice(const_cast<KisTransformMask*>(this), src,
                                staticCacheDevice, m_d->paramsHolder->isAnimated());
        QRect updatedRect = staticCacheDevice->extent();
        KisPainter::copyAreaOptimized(updatedRect.topLeft(), staticCacheDevice, dst, updatedRect);

        m_d->staticCache.setDeviceCacheValid(params);

#ifdef DEBUG_RENDERING
        qDebug() << "Recalculate" << name() << ppVar(src->exactBounds()) << ppVar(dst->exactBounds()) << ppVar(rc);
        KIS_DUMP_DEVICE_2(src, DUMP_RECT, "recalc_src", "dd");
        KIS_DUMP_DEVICE_2(dst, DUMP_RECT, "recalc_dst", "dd");
#endif

    } else if (params->isAffine() && !m_d->staticCache.isCacheValid(params)) {
        m_d->worker.setForceSubPixelTranslation(m_d->paramsHolder->isAnimated());
        m_d->worker.setForwardTransform(params->finalAffineTransform());
        m_d->worker.runPartialDst(src, dst, rc);

#ifdef DEBUG_RENDERING
        qDebug() << "Partial" << name() << ppVar(src->exactBounds()) << ppVar(src->extent()) << ppVar(dst->exactBounds()) << ppVar(dst->extent()) << ppVar(rc);
        KIS_DUMP_DEVICE_2(src, DUMP_RECT, "partial_src", "dd");
        KIS_DUMP_DEVICE_2(dst, DUMP_RECT, "partial_dst", "dd");
#endif

    } else if (m_d->staticCache.isCacheValid(params)) {
        KisPainter::copyAreaOptimized(rc.topLeft(), m_d->staticCache.device(), dst, rc);

#ifdef DEBUG_RENDERING
        qDebug() << "Fetch" << name() << ppVar(src->exactBounds()) << ppVar(dst->exactBounds()) << ppVar(rc);
        KIS_DUMP_DEVICE_2(src, DUMP_RECT, "fetch_src", "dd");
        KIS_DUMP_DEVICE_2(dst, DUMP_RECT, "fetch_dst", "dd");
#endif

    }

    KIS_ASSERT_RECOVER_NOOP(this->busyProgressIndicator());
    this->busyProgressIndicator()->update();

    return rc;
}

bool KisTransformMask::accept(KisNodeVisitor &v)
{
    return v.visit(this);
}

void KisTransformMask::accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter)
{
    return visitor.visit(this, undoAdapter);
}

namespace {
QRect calculateInterestRect(KisNodeSP node) {
    QRect resultInterestRect;

    if (KisGroupLayer *group = qobject_cast<KisGroupLayer*>(node.data())) {
        resultInterestRect = group->calculateChildrenLooseUserVisibleBounds();
    } else {
        resultInterestRect = node->original()->extent();
    }

    return resultInterestRect;
}
}

QRect KisTransformMask::changeRect(const QRect &rect, PositionToFilthy pos) const
{
    Q_UNUSED(pos);

    if (rect.isEmpty()) return rect;

    KisTransformMaskParamsInterfaceSP params = m_d->paramsHolder->bakeIntoParams();

    QRect changeRect = rect;

    if (params->isAffine()) {
        QRect bounds;
        QRect interestRect;
        KisNodeSP parentNode = parent();

        if (parentNode) {
            bounds = parentNode->original()->defaultBounds()->bounds();
            interestRect = calculateInterestRect(parentNode);
        } else {
            bounds = QRect(0,0,777,777);
            interestRect = QRect(0,0,888,888);
            warnKrita << "WARNING: transform mask has no parent (change rect)."
                      << "Cannot run safe transformations."
                      << "Will limit bounds to" << ppVar(bounds);
        }

        const QRect limitingRect = KisAlgebra2D::blowRect(bounds, m_d->offBoundsReadArea);

        KisSafeTransform transform(params->finalAffineTransform(), limitingRect, interestRect);
        changeRect = transform.mapRectForward(rect);
    } else {
        changeRect = params->nonAffineChangeRect(rect);
    }

    return changeRect;
}

QRect KisTransformMask::needRect(const QRect& rect, PositionToFilthy pos) const
{
    Q_UNUSED(pos);

    if (rect.isEmpty()) return rect;

    KisTransformMaskParamsInterfaceSP params = m_d->paramsHolder->bakeIntoParams();

    QRect bounds;
    QRect interestRect;
    KisNodeSP parentNode = parent();

    if (parentNode) {
        bounds = parentNode->original()->defaultBounds()->bounds();
        interestRect = calculateInterestRect(parentNode);
    } else {
        bounds = QRect(0,0,777,777);
        interestRect = QRect(0,0,888,888);
        warnKrita << "WARNING: transform mask has no parent (need rect)."
                   << "Cannot run safe transformations."
                   << "Will limit bounds to" << ppVar(bounds);
    }

    QRect needRect = rect;

    if (params->isAffine()) {
        const QRect limitingRect = KisAlgebra2D::blowRect(bounds, m_d->offBoundsReadArea);

        KisSafeTransform transform(params->finalAffineTransform(), limitingRect, interestRect);
        needRect = transform.mapRectBackward(rect);

        needRect = kisGrowRect(needRect, 1);

    } else {
        needRect = params->nonAffineNeedRect(rect, interestRect);
    }

    return needRect;
}

QRect KisTransformMask::extent() const
{
    QRect rc = KisMask::extent();

    QRect partialChangeRect;
    QRect existentProjection;
    KisLayerSP parentLayer = qobject_cast<KisLayer*>(parent().data());
    if (parentLayer) {
        partialChangeRect = parentLayer->partialChangeRect(const_cast<KisTransformMask*>(this), rc);
        existentProjection = parentLayer->projection()->extent();
    }

    return changeRect(partialChangeRect) | existentProjection;
}

QRect KisTransformMask::exactBounds() const
{
    QRect existentProjection;
    KisLayerSP parentLayer = qobject_cast<KisLayer*>(parent().data());
    if (parentLayer) {
        existentProjection = parentLayer->projection()->exactBounds();

        if (parentLayer->original() && parentLayer->original()->defaultBounds() && parentLayer->original()->keyframeChannel()) {
            Q_FOREACH( const int& time, parentLayer->original()->keyframeChannel()->allKeyframeTimes() ) {
                KisRasterKeyframeSP keyframe = parentLayer->original()->keyframeChannel()->keyframeAt<KisRasterKeyframe>(time);
                existentProjection |= keyframe->contentBounds();
            }
        }
    }

    if (isAnimated()) {
        existentProjection |= changeRect(image()->bounds());
    }

    return changeRect(sourceDataBounds()) | existentProjection;
}

QRect KisTransformMask::sourceDataBounds() const
{

    QRect partialChangeRect;
    KisLayerSP parentLayer = qobject_cast<KisLayer*>(parent().data());
    if (parentLayer) {
        const QRect rc = parentLayer->original()->exactBounds();
        partialChangeRect = parentLayer->partialChangeRect(const_cast<KisTransformMask*>(this), rc);
    }

    return partialChangeRect;
}

void KisTransformMask::setImage(KisImageWSP image)
{
    m_d->paramsHolder->setDefaultBounds(new KisDefaultBounds(image));
    m_d->offset.setDefaultBounds(new KisDefaultBounds(image));
    KisEffectMask::setImage(image);
}

qint32 KisTransformMask::x() const
{
    return m_d->offset->x();
}

qint32 KisTransformMask::y() const
{
    return m_d->offset->y();
}

void KisTransformMask::setX(qint32 x)
{
    KisTransformMaskParamsInterfaceSP params(m_d->paramsHolder->bakeIntoParams());

    params->translateSrcAndDst(QPointF(x - this->x(), 0));

    setTransformParams(params);
    m_d->offset->setX(x);
}

void KisTransformMask::setY(qint32 y)
{
    KisTransformMaskParamsInterfaceSP params(m_d->paramsHolder->bakeIntoParams());

    params->translateSrcAndDst(QPointF(0, y - this->y()));

    setTransformParams(params);
    m_d->offset->setY(y);
}

void KisTransformMask::forceUpdateTimedNode()
{
    if (m_d->testingInterface) {
        m_d->testingInterface->notifyForceUpdateTimedNode();
    }

    if (hasPendingTimedUpdates() ||
        !m_d->staticCache.isCacheValid(m_d->paramsHolder->bakeIntoParams())) {

        forceStartAsyncRegenerationJob();
    }
}

bool KisTransformMask::hasPendingTimedUpdates() const
{
    return m_d->updateSignalCompressor.isActive();
}

void KisTransformMask::threadSafeForceStaticImageUpdate()
{
    threadSafeForceStaticImageUpdate(QRect());
}

void KisTransformMask::threadSafeForceStaticImageUpdate(const QRect &extraUpdateRect)
{
    if (m_d->testingInterface) {
        m_d->testingInterface->notifyThreadSafeForceStaticImageUpdate();
    }
    if (!extraUpdateRect.isEmpty()) {
        m_d->forcedStaticUpdateExtraUpdateRect.addRect(extraUpdateRect);
    }
    Q_EMIT sigInternalForceStaticImageUpdate();
}

void KisTransformMask::slotInternalForceStaticImageUpdate()
{
    forceStartAsyncRegenerationJob();
}

void KisTransformMask::syncLodCache()
{
    m_d->offset.syncLodCache();
    m_d->paramsHolder->syncLodCache();
    KisEffectMask::syncLodCache();
}

KisPaintDeviceList KisTransformMask::getLodCapableDevices() const
{
    KisPaintDeviceList devices;
    devices += KisEffectMask::getLodCapableDevices();
    if (m_d->staticCache.device()) {
        devices << m_d->staticCache.device();
    }
    return devices;
}

void KisTransformMask::setTestingInterface(KisTransformMaskTestingInterface *interface)
{
    m_d->testingInterface.reset(interface);
}

KisTransformMaskTestingInterface* KisTransformMask::testingInterface() const
{
    return m_d->testingInterface.data();
}

KisKeyframeChannel *KisTransformMask::requestKeyframeChannel(const QString &id)
{
    if (id == KisKeyframeChannel::PositionX.id() ||
        id == KisKeyframeChannel::PositionY.id() ||
        id == KisKeyframeChannel::ScaleX.id() ||
        id == KisKeyframeChannel::ScaleY.id() ||
        id == KisKeyframeChannel::ShearX.id() ||
        id == KisKeyframeChannel::ShearY.id() ||
        id == KisKeyframeChannel::RotationX.id() ||
        id == KisKeyframeChannel::RotationY.id() ||
        id == KisKeyframeChannel::RotationZ.id()) {

        KisKeyframeChannel *channel = m_d->paramsHolder->requestKeyframeChannel(id);
        KIS_SAFE_ASSERT_RECOVER_NOOP(channel);
        return channel;
    }

    return KisEffectMask::requestKeyframeChannel(id);
}

bool KisTransformMask::supportsKeyframeChannel(const QString &id)
{
    if (id == KisKeyframeChannel::PositionX.id() ||
        id == KisKeyframeChannel::PositionY.id() ||
        id == KisKeyframeChannel::ScaleX.id() ||
        id == KisKeyframeChannel::ScaleY.id() ||
        id == KisKeyframeChannel::ShearX.id() ||
        id == KisKeyframeChannel::ShearY.id() ||
        id == KisKeyframeChannel::RotationX.id() ||
        id == KisKeyframeChannel::RotationY.id() ||
            id == KisKeyframeChannel::RotationZ.id()) {
        return true;
    }
    else if (id == KisKeyframeChannel::Opacity.id()) {
        return false;
    }

    return KisEffectMask::supportsKeyframeChannel(id);
}

