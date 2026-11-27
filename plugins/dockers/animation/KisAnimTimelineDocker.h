/*
 *  SPDX-FileCopyrightText: 2015 Jouni Pentikäinen <joupent@gmail.com>
 *  SPDX-FileCopyrightText: 2020 Emmet O 'Neill <emmetoneill.pdx@gmail.com>
 *  SPDX-FileCopyrightText: 2020 Eoin O 'Neill <eoinoneill1991@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _TIMELINE_DOCKER_H_
#define _TIMELINE_DOCKER_H_

#include "kritaimage_export.h"

#include <QScopedPointer>
#include <QDockWidget>

#include <kis_mainwindow_observer.h>
#include <kis_utility_title_bar.h>

#ifdef Q_OS_MACOS
#include <sys/types.h>
#endif

class QToolButton;
class KisTransportControls;
class KisIntParseSpinBox;
class KisSliderSpinBox;

class KisCanvas2;
class KisAction;
class KisActionManager;
class KisPlaybackEngine;

class KisAnimTimelineDockerTitlebar : public KisUtilityTitleBar
{
    Q_OBJECT

public:
    KisAnimTimelineDockerTitlebar(QWidget *parent = nullptr);

    KisTransportControls* transport;

    KisIntParseSpinBox *frameRegister;

    QToolButton *btnAddKeyframe;
    QToolButton *btnDuplicateKeyframe;
    QToolButton *btnRemoveKeyframe;

    QToolButton *btnOnionSkinsMenu;
    QToolButton *btnAudioMenu;
    QToolButton *btnSettingsMenu;

    QString strImportAudio;
    QAction *importAudioAction;
    QAction *removeAudioAction;
    QAction *muteAudioAction;
    KisSliderSpinBox *volumeSlider;

    KisIntParseSpinBox *sbStartFrame;
    KisIntParseSpinBox *sbEndFrame;
    KisIntParseSpinBox *sbFrameRate;
    KisSliderSpinBox *sbSpeed;

    QToolButton *btnDropFrames;

    QToolButton *btnAutoKey;
    QAction *autoKeyBlank;
    QAction *autoKeyDuplicate;

private:
    const int MAX_FRAMES = 9999;
};

class KisAnimTimelineDocker : public QDockWidget, public KisMainwindowObserver
{
    Q_OBJECT
public:
    KisAnimTimelineDocker();
    ~KisAnimTimelineDocker() override;

    QString observerName() override { return "TimelineDocker"; }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;
    void setViewManager(KisViewManager *kisview) override;
    void setPlaybackEngine(KisPlaybackEngine *playbackEngine);

public Q_SLOTS:
    void togglePlayback();
    void setAutoKey(bool value);

    void handleFrameRateChange();

    void updateFrameCache();
    void updateFrameRegister();
    void updatePlaybackStatistics();

    void handleThemeChange();

    void setImageAnimSettings();

private:
    void establishPlaybackEngineConnections(KisPlaybackEngine *playbackEngine, KisActionManager *actionManager = nullptr);

    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
