/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_REGENERATE_FRAME_STROKE_STRATEGY_H
#define __KIS_REGENERATE_FRAME_STROKE_STRATEGY_H

#include <kis_simple_stroke_strategy.h>

#include <QScopedPointer>

class KisRegion;

class KisImageAnimationInterface;
class KisLockFrameGenerationLock;

class KisRegenerateFrameStrokeStrategy : public KisSimpleStrokeStrategy
{
public:
    enum Type {
        EXTERNAL_FRAME,
        CURRENT_FRAME
    };

public:
    KisRegenerateFrameStrokeStrategy(int frameId,
                                     const KisRegion &dirtyRegion,
                                     bool isCancellable,
                                     KisImageAnimationInterface *interface,
                                     KisLockFrameGenerationLock &&frameGenerationLock);

    KisRegenerateFrameStrokeStrategy(KisImageAnimationInterface *interface);

    ~KisRegenerateFrameStrokeStrategy() override;

    void initStrokeCallback() override;
    void doStrokeCallback(KisStrokeJobData *data) override;
    void finishStrokeCallback() override;
    void cancelStrokeCallback() override;

    KisStrokeStrategy* createLodClone(int levelOfDetail) override;
    void suspendStrokeCallback() override;
    void resumeStrokeCallback() override;

    static QList<KisStrokeJobData*> createJobsData(KisImageWSP image);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
