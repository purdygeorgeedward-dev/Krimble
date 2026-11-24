/*
 *  SPDX-FileCopyrightText: 2015 Jouni Pentikäinen <joupent@gmail.com>
 *  SPDX-FileCopyrightText: 2021 Eoin O'Neill <eoinoneill1991@gmail.com>
 *  SPDX-FileCopyrightText: 2021 Emmet O'Neill <emmetoneill.pdx@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ANIMATION_PLAYER_H
#define KIS_ANIMATION_PLAYER_H

#include <QScopedPointer>
#include <QObject>
#include <QFileInfo>

#include <kis_time_span.h>
#include <boost/optional.hpp>

#include "kritaui_export.h"

class KisCanvas2;

enum PlaybackState : unsigned int {
    STOPPED,
    PAUSED,
    PLAYING
};

class KRITAUI_EXPORT KisCanvasAnimationState : public QObject
{
    Q_OBJECT

public:
    KisCanvasAnimationState(KisCanvas2 *canvas);
    ~KisCanvasAnimationState() override;

    void setPlaybackState(PlaybackState state);
    PlaybackState playbackState();

    boost::optional<QFileInfo> mediaInfo();
    qreal currentVolume();

    boost::optional<int> playbackOrigin();

    class KisFrameDisplayProxy *displayProxy();

    void setPlaybackSpeed(qreal value);
    qreal playbackSpeed() const;

    void setupAudioTracks();

public Q_SLOTS:
    void showFrame(int frame, bool finalize = false);

Q_SIGNALS:
    void sigPlaybackStateChanged(PlaybackState state);
    void sigPlaybackStatisticsUpdated();
    void sigPlaybackSpeedChanged(qreal value);
    void sigFrameChanged();
    void sigPlaybackMediaChanged();
    void sigAudioLevelChanged(qreal value);

    void sigCancelPlayback();

private:
    KisTimeSpan activePlaybackRange();

    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
