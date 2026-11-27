/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISASYNCANIMATIONRENDERERBASE_H
#define KISASYNCANIMATIONRENDERERBASE_H

#include <QObject>
#include "kis_types.h"

#include "kritaui_export.h"

class KisRegion;
class KisLockFrameGenerationLock;

class KRITAUI_EXPORT KisAsyncAnimationRendererBase : public QObject
{
    Q_OBJECT

public:
    enum Flag
    {
        None = 0x0,
        Cancellable = 0x1
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    enum CancelReason {
        UserCancelled = 0,
        RenderingFailed,
        RenderingTimedOut
    };
    Q_ENUM(CancelReason)

public:
    explicit KisAsyncAnimationRendererBase(QObject *parent = 0);
    virtual ~KisAsyncAnimationRendererBase();

    void startFrameRegeneration(KisImageSP image, int frame, const KisRegion &regionOfInterest, Flags flags, KisLockFrameGenerationLock &&frameGenerationLock);

    void startFrameRegeneration(KisImageSP image, int frame, Flags flags, KisLockFrameGenerationLock &&frameGenerationLock);

    bool isActive() const;

public Q_SLOTS:
    void cancelCurrentFrameRendering(CancelReason cancelReason);

Q_SIGNALS:
    void sigFrameCompleted(int frame);
    void sigFrameCancelled(int frame, KisAsyncAnimationRendererBase::CancelReason cancelReason);

private Q_SLOTS:
    void slotFrameRegenerationCancelled();
    void slotFrameRegenerationTimedOut();
    void slotFrameRegenerationFinished(int frame);

protected Q_SLOTS:
    void notifyFrameCompleted(int frame);

    void notifyFrameCancelled(int frame, KisAsyncAnimationRendererBase::CancelReason cancelReason);

protected:
    virtual void frameCompletedCallback(int frame, const KisRegion &requestedRegion) = 0;

    virtual void frameCancelledCallback(int frame, CancelReason cancelReason) = 0;

    virtual void clearFrameRegenerationState(bool isCancelled);

protected:
    KisImageSP requestedImage() const;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisAsyncAnimationRendererBase::Flags)

#endif
