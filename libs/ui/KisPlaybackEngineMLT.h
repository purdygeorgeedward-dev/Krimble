/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Emmet O'Neill <emmetoneill.pdx@gmail.com>
   SPDX-FileCopyrightText: 2022 Eoin O'Neill <eoinoneill1991@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KISPLAYBACKENGINEMLT_H
#define KISPLAYBACKENGINEMLT_H

#include <QObject>
#include "KoCanvasObserverBase.h"
#include "KisPlaybackEngine.h"
#include <kritaui_export.h>

#include <QScopedPointer>
#include <QFileInfo>
#include <boost/optional.hpp>

enum PlaybackMode {
    PLAYBACK_PUSH,
    PLAYBACK_PULL
};

class KRITAUI_EXPORT KisPlaybackEngineMLT : public KisPlaybackEngine
{
    Q_OBJECT
public:
    explicit KisPlaybackEngineMLT(QObject *parent = nullptr);
    ~KisPlaybackEngineMLT();

Q_SIGNALS:
    void sigChangeActiveCanvasFrame(int p_frame);

public Q_SLOTS:
    void seek(int frameIndex, SeekOptionFlags flags = SEEK_FINALIZE | SEEK_PUSH_AUDIO) override;

    void setMute(bool val) override;
    bool isMute() override;

    bool supportsAudio() override { return true; }
    bool supportsVariablePlaybackSpeed() override { return true; }

    void setDropFramesMode(bool value) override;

    PlaybackStats playbackStatistics() const override;

protected Q_SLOTS:
    void setCanvas(KoCanvasBase* canvas) override;
    void unsetCanvas() override;
    void canvasDestroyed(QObject *canvas);

    void throttledShowFrame(const int frame);

    void throttledSetSpeed(const double speed);

    void setAudioVolume(qreal volumeNormalized);

public:
    struct FrameWaitingInterface;
    FrameWaitingInterface* frameWaitingInterface();

private:
    void setupProducer(boost::optional<QFileInfo> file);

    struct Private;
    struct StopAndResume;
    QScopedPointer<Private> m_d;
};

#endif
