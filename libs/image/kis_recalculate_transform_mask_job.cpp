/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_recalculate_transform_mask_job.h"

#include "kis_transform_mask.h"
#include "kis_debug.h"
#include "kis_layer.h"
#include "kis_image.h"
#include "kis_image_animation_interface.h"
#include "kis_abstract_projection_plane.h"
#include "kis_transform_mask_params_interface.h"

KisRecalculateTransformMaskJob::KisRecalculateTransformMaskJob(KisTransformMaskSP mask, const QRect &extraUpdateRect)
    : m_mask(mask)
    , m_extraUpdateRect(extraUpdateRect)
{
    setExclusive(true);
}

bool KisRecalculateTransformMaskJob::overrides(const KisSpontaneousJob *_otherJob)
{
    const KisRecalculateTransformMaskJob *otherJob =
        dynamic_cast<const KisRecalculateTransformMaskJob*>(_otherJob);

    return otherJob && otherJob->m_mask == m_mask;
}

void KisRecalculateTransformMaskJob::run()
{
    if (!m_mask->parent()) return;
    if (!m_mask->visible()) return;
    if (m_mask->staticImageCacheIsValid()) return;

    const QRect oldMaskExtent = m_mask->extent();
    m_mask->recalculateStaticImage();

    KisLayerSP layer = qobject_cast<KisLayer*>(m_mask->parent().data());

    if (!layer) {
        warnKrita << "WARNING: KisRecalculateTransformMaskJob::run() Mask has no parent layer! Skipping projection update!";
        return;
    }

    KisImageSP image = layer->image();
    Q_ASSERT(image);

    if (m_mask->transformParams()->isHidden()) {
        QRect updateRect = m_mask->extent() | oldMaskExtent;

        updateRect |= m_extraUpdateRect;

        if (layer->original()) {
            updateRect |= layer->original()->defaultBounds()->bounds();
        }

        if (layer->isAnimated()) {
            m_mask->setDirty(updateRect);
        } else {
            m_mask->setDirtyDontResetAnimationCache(updateRect);
        }
    } else {
        QRect updateRect = oldMaskExtent |
            layer->projectionPlane()->changeRect(layer->extent(), KisLayer::N_FILTHY);

        updateRect |= m_extraUpdateRect;

        image->requestProjectionUpdate(layer.data(), {updateRect},
                                       KisProjectionUpdateFlag::NoFilthy |
                                           KisProjectionUpdateFlag::DontInvalidateFrames);
    }
}

int KisRecalculateTransformMaskJob::levelOfDetail() const
{
    return 0;
}

QString KisRecalculateTransformMaskJob::debugName() const
{
    QString result;
    QDebug dbg(&result);
    dbg << "KisRecalculateTransformMaskJob" << m_mask;
    return result;
}
