/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISASYNCANIMATIONRENDERDIALOGBASE_H
#define KISASYNCANIMATIONRENDERDIALOGBASE_H

#include <QObject>
#include "kis_types.h"
#include "kritaui_export.h"

#include <KisAsyncAnimationRendererBase.h>

class KisTimeSpan;
class KisViewManager;
class KisRegion;

class KRITAUI_EXPORT KisAsyncAnimationRenderDialogBase : public QObject
{
    Q_OBJECT
public:
    enum Result {
        RenderComplete,
        RenderCancelled,
        RenderFailed,
        RenderTimedOut
    };

public:
    KisAsyncAnimationRenderDialogBase(const QString &actionTitle, KisImageSP image, int busyWait = 200);
    virtual ~KisAsyncAnimationRenderDialogBase();

    virtual Result regenerateRange(KisViewManager *viewManager);

    void setRegionOfInterest(const KisRegion &roi);

    KisRegion regionOfInterest() const;

    void setBatchMode(bool value);

    bool batchMode() const;

private Q_SLOTS:
    void slotFrameCompleted(int frame);
    void slotFrameCancelled(int frame, KisAsyncAnimationRendererBase::CancelReason cancelReason);

    void slotCancelRegeneration();
    void slotUpdateCompressedProgressData();

private:
    void tryInitiateFrameRegeneration();
    void updateProgressLabel();
    void cancelProcessingImpl(KisAsyncAnimationRendererBase::CancelReason cancelReason);

protected:
    virtual QList<int> calcDirtyFrames() const = 0;

    virtual KisAsyncAnimationRendererBase* createRenderer(KisImageSP image) = 0;

    virtual void initializeRendererForFrame(KisAsyncAnimationRendererBase *renderer,
                                            KisImageSP image, int frame) = 0;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
