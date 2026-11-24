/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Emmet O'Neill <emmetoneill.pdx@gmail.com>
   SPDX-FileCopyrightText: 2022 Eoin O'Neill <eoinoneill1991@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KISPLAYBACKENGINEQT_H
#define KISPLAYBACKENGINEQT_H

#include "KisPlaybackEngine.h"

#include <kritaui_export.h>

#include "canvas/KisCanvasAnimationState.h"
#include <boost/optional.hpp>

class KRITAUI_EXPORT KisPlaybackEngineQT : public KisPlaybackEngine
{
    Q_OBJECT

public:
    explicit KisPlaybackEngineQT(QObject *parent = nullptr);
    ~KisPlaybackEngineQT();

    void seek(int frameIndex, SeekOptionFlags flags = SEEK_FINALIZE | SEEK_PUSH_AUDIO) override;

    void setMute(bool) override {}
    bool isMute() override { return true; }

    bool supportsAudio() override {return false;}
    bool supportsVariablePlaybackSpeed() override { return true; }

    void setDropFramesMode(bool value) override;

    boost::optional<int64_t> activeFramesPerSecond() const;

    PlaybackStats playbackStatistics() const override;

protected Q_SLOTS:
    void throttledDriverCallback();

protected:
    void setCanvas(KoCanvasBase* canvas) override;
    void unsetCanvas() override;

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
