/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_IMAGE_ANIMATION_INTERFACE_H
#define __KIS_IMAGE_ANIMATION_INTERFACE_H

#include <QObject>
#include <QScopedPointer>

#include "kis_types.h"
#include "kritaimage_export.h"

class KisUpdatesFacade;
class KisTimeSpan;
class KisKeyframeChannel;
class KoColor;
class KisRegion;
class KisLockFrameGenerationLock;

namespace KisLayerUtils {
    struct SwitchFrameCommand;
}

class KRITAIMAGE_EXPORT KisImageAnimationInterface : public QObject
{
    Q_OBJECT

public:
    KisImageAnimationInterface(KisImage *image);
    KisImageAnimationInterface(const KisImageAnimationInterface &rhs, KisImage *newImage);
    ~KisImageAnimationInterface() override;

    bool hasAnimation() const;

    int currentTime() const;

    int currentUITime() const;

    bool externalFrameActive() const;

    void requestTimeSwitchWithUndo(int time);

    void requestTimeSwitchNonGUI(int time, bool useUndo = false);

    void requestFrameRegeneration(int frameId, const KisRegion &dirtyRegion, bool isCancellable, KisLockFrameGenerationLock &&lock);

    void notifyNodeChanged(const KisNode *node, const QRect &rect, bool recursive);
    void notifyNodeChanged(const KisNode *node, const QVector<QRect> &rects, bool recursive);
    void invalidateFrames(const KisTimeSpan &range, const QRect &rect);
    void invalidateFrame(const int time, KisNodeSP target);

    void setDefaultProjectionColor(const KoColor &color);

    const KisTimeSpan& documentPlaybackRange() const;
    void setDocumentRange(const KisTimeSpan range);

    const KisTimeSpan &activePlaybackRange() const;
    void setActivePlaybackRange(const KisTimeSpan range);

    int framerate() const;

    QString exportSequenceFilePath();
    void setExportSequenceFilePath(const QString &filePath);

    QString exportSequenceBaseName();
    void setExportSequenceBaseName(const QString &baseName);

    int exportInitialFrameNumber();
    void setExportInitialFrameNumber(const int frameNum);

    QSet<int> activeLayerSelectedTimes();
    void setActiveLayerSelectedTimes(const QSet<int> &times);

    KisImageWSP image() const;

    int totalLength();

    void blockBackgroundFrameGeneration();

    void unblockBackgroundFrameGeneration();

    bool backgroundFrameGenerationBlocked() const;

    void lockFrameGeneration();

    void unlockFrameGeneration();

    bool tryLockFrameGeneration();

    enum SwitchTimeAsyncOption {
        STAO_NONE = 0,
        STAO_USE_UNDO = 1 << 1,
        STAO_FORCE_REGENERATION = 1 << 2
    };
    Q_DECLARE_FLAGS(SwitchTimeAsyncFlags, SwitchTimeAsyncOption);

public Q_SLOTS:

    void switchCurrentTimeAsync(int frameId, SwitchTimeAsyncFlags options = STAO_NONE);

    void setDocumentRangeStartFrame(int column);
    void setDocumentRangeEndFrame(int column);

    void setFramerate(int fps);

Q_SIGNALS:
    void sigFrameReady(int time);

    void sigFrameRegenerated(int time);

    void sigFrameRegenerationSkipped(int time);

    void sigFrameCancelled();
    void sigUiTimeChanged(int newTime);
    void sigFramesChanged(const KisTimeSpan &range, const QRect &rect);

    void sigInternalRequestTimeSwitch(int frameId, bool useUndo);

    void sigFramerateChanged();
    void sigPlaybackRangeChanged();
    void sigDocumentRangeChanged();

    void sigKeyframeAdded(const KisKeyframeChannel* channel, int time);
    void sigKeyframeRemoved(const KisKeyframeChannel* channel, int time);

private:
    friend class KisRegenerateFrameStrokeStrategy;
    friend class KisSuspendProjectionUpdatesStrokeStrategy;
    friend class KisAnimationFrameCacheTest;
    friend struct KisLayerUtils::SwitchFrameCommand;
    friend class KisImageTest;
    void saveAndResetCurrentTime(int frameId, int *savedValue);
    void restoreCurrentTime(int *savedValue);
    void notifyFrameReady();
    void notifyFrameCancelled();
    void notifyFrameRegenerated();
    bool requiresOnionSkinRendering();

    KisUpdatesFacade* updatesFacade() const;

    void blockFrameInvalidation(bool value);

    friend class KisSwitchTimeStrokeStrategy;
    friend class TransformStrokeStrategy;
    void explicitlySetCurrentTime(int frameId);
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
