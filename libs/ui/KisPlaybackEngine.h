/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Emmet O'Neill <emmetoneill.pdx@gmail.com>
   SPDX-FileCopyrightText: 2022 Eoin O'Neill <eoinoneill1991@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KISPLAYBACKENGINE_H
#define KISPLAYBACKENGINE_H

#include "KoCanvasObserverBase.h"
#include <QObject>

#include <kritaui_export.h>

enum SeekOption {
    SEEK_NONE = 0,
    SEEK_PUSH_AUDIO = 1,
    SEEK_FINALIZE = 1 << 1
};

Q_DECLARE_FLAGS(SeekOptionFlags, SeekOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(SeekOptionFlags)

class KRITAUI_EXPORT KisPlaybackEngine : public QObject, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    KisPlaybackEngine(QObject* parent = nullptr);
    ~KisPlaybackEngine();

    struct PlaybackStats {
        qreal expectedFps {0.0};
        qreal realFps {0.0};
        qreal droppedFramesPortion {0.0};
    };

public Q_SLOTS:
    virtual void play();
    virtual void pause();
    virtual void playPause();
    virtual void stop();

    virtual void seek( int frameIndex, SeekOptionFlags options = SEEK_FINALIZE | SEEK_PUSH_AUDIO ) = 0;
    virtual void previousFrame();
    virtual void nextFrame();
    virtual void previousKeyframe();
    virtual void nextKeyframe();
    virtual void firstFrame();
    virtual void lastFrame();

    virtual void previousMatchingKeyframe();
    virtual void nextMatchingKeyframe();

    virtual void previousUnfilteredKeyframe();
    virtual void nextUnfilteredKeyframe();

    virtual void setMute(bool val) = 0;
    virtual bool isMute() = 0;

    virtual void setDropFramesMode(bool value);
    bool dropFrames() const;

    virtual bool supportsAudio() = 0;
    virtual bool supportsVariablePlaybackSpeed() = 0;

    virtual PlaybackStats playbackStatistics() const = 0;

Q_SIGNALS:
    void sigDropFramesModeChanged(bool value);

protected:
    class KisCanvas2* activeCanvas() const;
    int frameWrap(int frame, int startFrame, int endFrame);

protected Q_SLOTS:
    virtual void setCanvas(KoCanvasBase* p_canvas) override;
    virtual void unsetCanvas() override;

private:
    void moveActiveFrameBy(int frames);

    void nextKeyframeWithColor(int color);
    void nextKeyframeWithColor(const QSet<int> &validColors);
    void previousKeyframeWithColor(int color);
    void previousKeyframeWithColor(const QSet<int> &validColors);

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
