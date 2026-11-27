/*
 *  SPDX-FileCopyrightText: 2022 Eoin O'Neill <eoinoneill1991@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Emmet O'Neill <emmetoneill.pdx@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisFrameDisplayProxy.h"

#include "kis_canvas2.h"
#include "kis_image_animation_interface.h"
#include "KisCanvasAnimationState.h"

struct Private {
    Private(KisCanvas2* c)
        : intendedFrame(0)
        , displayedFrame(-1)
        , canvas(c) {}

    int intendedFrame;
    int displayedFrame;
    KisCanvas2 *canvas;
};

KisFrameDisplayProxy::KisFrameDisplayProxy(KisCanvas2* canvas, QObject *parent)
    : QObject(parent)
    , m_d(new Private(canvas))
{
    KIS_ASSERT(canvas);

    connect(m_d->canvas->image()->animationInterface(), &KisImageAnimationInterface::sigFrameRegenerated, this, [this](int frame){
        if (m_d->intendedFrame != frame ) {
            KisCanvasAnimationState* state = m_d->canvas->animationState();
            if (state->playbackState() != PLAYING) {
                m_d->intendedFrame = frame;
                Q_EMIT sigFrameChange();
            }
        }

        if (m_d->displayedFrame != frame) {
            m_d->displayedFrame = frame;
            Q_EMIT sigFrameDisplayRefreshed();
        }
    });

    connect(m_d->canvas->image()->animationInterface(), &KisImageAnimationInterface::sigFrameRegenerationSkipped, this, [this](int frame){
       if (m_d->intendedFrame != frame) {
           KisCanvasAnimationState* state = m_d->canvas->animationState();
           if (state->playbackState() != PLAYING) {
               m_d->intendedFrame = frame;
               Q_EMIT sigFrameChange();
           }
       }

       Q_EMIT sigFrameRefreshSkipped();
    });

    m_d->displayedFrame = m_d->canvas->image()->animationInterface()->currentUITime();
    m_d->intendedFrame = m_d->displayedFrame;
}

KisFrameDisplayProxy::~KisFrameDisplayProxy()
{
}

bool KisFrameDisplayProxy::displayFrame(int frame, bool forceReproject)
{
    KisAnimationFrameCacheSP cache = m_d->canvas->frameCache();
    KisImageAnimationInterface* ai = m_d->canvas->image()->animationInterface();

    if (frame != m_d->intendedFrame) {
        m_d->intendedFrame = frame;
        Q_EMIT sigFrameChange();
    }

    if (forceReproject || needsReprojection(cache, m_d->displayedFrame, frame)) {
        ai->switchCurrentTimeAsync(frame, KisImageAnimationInterface::STAO_FORCE_REGENERATION);
        return true;

    } else if ( shouldUploadFrame(cache, m_d->displayedFrame, frame) && cache->uploadFrame(frame) ) {
        m_d->canvas->updateCanvas();
        m_d->displayedFrame = frame;
        Q_EMIT sigFrameDisplayRefreshed();
        return true;

    } else if (!cache && ai->hasAnimation() && ai->currentUITime() != frame){
        if (m_d->canvas->image()->tryBarrierLock(true)) {
            m_d->canvas->image()->unlock();
            ai->switchCurrentTimeAsync(frame);
            return true;
        }
    }

    return false;
}

int KisFrameDisplayProxy::activeFrame() const
{
    return m_d->intendedFrame;
}

int KisFrameDisplayProxy::activeKeyframe() const
{
    return m_d->displayedFrame;
}

bool KisFrameDisplayProxy::shouldUploadFrame(KisAnimationFrameCacheSP cache, int from, int to)
{
    return cache && cache->shouldUploadNewFrame(to, from);
}

bool KisFrameDisplayProxy::needsReprojection(KisAnimationFrameCacheSP cache, int from, int to)
{
    return cache && cache->frameStatus(from) != cache->frameStatus(to);
}
