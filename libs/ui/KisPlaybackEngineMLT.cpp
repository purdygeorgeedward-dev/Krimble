/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Emmet O'Neill <emmetoneill.pdx@gmail.com>
   SPDX-FileCopyrightText: 2022 Eoin O'Neill <eoinoneill1991@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KisPlaybackEngineMLT.h"

#include <QMap>

#include <QMutex>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QWaitCondition>

#include "kis_canvas2.h"
#include "KisCanvasAnimationState.h"
#include "kis_image_animation_interface.h"
#include "kis_raster_keyframe_channel.h"
#include "kis_signal_compressor_with_param.h"
#include "animation/KisFrameDisplayProxy.h"
#include "KisViewManager.h"
#include "kis_onion_skin_compositor.h"

#include <mlt++/Mlt.h>
#include <mlt++/MltConsumer.h>
#include <mlt++/MltFrame.h>
#include <mlt++/MltFilter.h>
#include <mlt-7/framework/mlt_service.h>

#include "KisRollingMeanAccumulatorWrapper.h"
#include "KisRollingSumAccumulatorWrapper.h"

#ifdef Q_OS_ANDROID
#include <KisAndroidFileProxy.h>
#endif

#include "kis_debug.h"

#include "KisMLTProducerKrita.h"

#ifdef MLT_LOG_REDIRECTION
void qt_redirection_callback(void *ptr, int level, const char *fmt, va_list vl)
{
    static int print_prefix = 1;
    mlt_properties properties = ptr ? MLT_SERVICE_PROPERTIES((mlt_service) ptr) : NULL;

    if (level > mlt_log_get_level())
        return;

    static const int prefix_size = 200;
    char prefix[prefix_size] = "";

    if (print_prefix && properties) {
        char *mlt_type = mlt_properties_get(properties, "mlt_type");
        char *mlt_service = mlt_properties_get(properties, "mlt_service");
        char *resource = mlt_properties_get(properties, "resource");

        if (!(resource && *resource && resource[0] == '<' && resource[strlen(resource) - 1] == '>'))
            mlt_type = mlt_properties_get(properties, "mlt_type");
        if (mlt_service)
            snprintf(prefix, prefix_size, "[%s %s] ", mlt_type, mlt_service);
        else
            snprintf(prefix, prefix_size, "[%s %p] ", mlt_type, ptr);
        if (resource)
            snprintf(prefix, prefix_size, "%s\n    ", resource);
        qDebug().nospace() << qPrintable(prefix);
    }
    print_prefix = strstr(fmt, "\n") != NULL;
    vsnprintf(prefix, prefix_size, fmt, vl);
    qDebug().nospace() << qPrintable(prefix);
}
#endif

const float SCRUB_AUDIO_SECONDS = 0.128f;

struct KisPlaybackEngineMLT::FrameWaitingInterface {
    bool renderingAllowed {false};
    bool waitingForFrame {false};
    QMutex renderingControlMutex;
    QWaitCondition renderingWaitCondition;
};

namespace {

struct FrameRenderingStats
{
    static constexpr int frameStatsWindow = 50;

    KisRollingMeanAccumulatorWrapper averageFrameDuration {frameStatsWindow};
    KisRollingSumAccumulatorWrapper droppedFramesCount {frameStatsWindow};
    int lastRenderedFrame {-1};
    QElapsedTimer timeSinceLastFrame;

    void reset() {
        averageFrameDuration.reset(frameStatsWindow);
        droppedFramesCount.reset(frameStatsWindow);
        lastRenderedFrame = -1;
    }
};

}
static void mltOnConsumerFrameShow(mlt_consumer c, void* p_self, mlt_frame p_frame) {
    KisPlaybackEngineMLT* self = static_cast<KisPlaybackEngineMLT*>(p_self);
    Mlt::Frame frame(p_frame);
    Mlt::Consumer consumer(c);
    const int position = frame.get_position();

    KisPlaybackEngineMLT::FrameWaitingInterface *iface = self->frameWaitingInterface();

    QMutexLocker l(&iface->renderingControlMutex);

    if (!iface->renderingAllowed) return;

    KIS_SAFE_ASSERT_RECOVER_RETURN(!iface->waitingForFrame);
    iface->waitingForFrame = true;

    Q_EMIT self->sigChangeActiveCanvasFrame(position);

    while (iface->renderingAllowed && iface->waitingForFrame) {
        iface->renderingWaitCondition.wait(&iface->renderingControlMutex);
    }
}

struct KisPlaybackEngineMLT::Private {

    Private(KisPlaybackEngineMLT* p_self)
        : m_self(p_self)
        , playbackSpeed(1.0)
        , mute(false)
    {
        qDebug() << "Initializing MLT Animation Playback Engine. MLT ver: " << QString(mlt_version_get_string());

        repository.reset(Mlt::Factory::init());

#ifdef MLT_LOG_REDIRECTION
        mlt_log_set_level(MLT_LOG_VERBOSE);
        mlt_log_set_callback(&qt_redirection_callback);
#endif

        registerKritaMLTProducer(repository.data());

        profile.reset(new Mlt::Profile());
        profile->set_frame_rate(24, 1);

        {
            std::function<void (int)> callback(std::bind(&Private::pushAudio, this, std::placeholders::_1));
            sigPushAudioCompressor.reset(
                        new KisSignalCompressorWithParam<int>(1000 * SCRUB_AUDIO_SECONDS, callback, KisSignalCompressor::FIRST_ACTIVE)
                        );
        }

        {
            std::function<void (const double)> callback(std::bind(&KisPlaybackEngineMLT::throttledSetSpeed, m_self, std::placeholders::_1));
            sigSetPlaybackSpeed.reset(
                        new KisSignalCompressorWithParam<double>(100, callback, KisSignalCompressor::POSTPONE)
                        );
        }

        initializeConsumers();
    }

    ~Private() {
        cleanupConsumers();
        repository.reset();
        Mlt::Factory::close();
    }

    void pushAudio(int frame) {

        if (pushConsumer->is_stopped() || !m_self->activeCanvas()) {
            return;
        }

        QSharedPointer<Mlt::Producer> activeProducer = canvasProducers[m_self->activeCanvas()];
        if (activePlaybackMode() == PLAYBACK_PUSH && activeProducer) {
            const int SCRUB_AUDIO_WINDOW = qMax(1, qRound(profile->frame_rate_num() * SCRUB_AUDIO_SECONDS));
            activeProducer->seek(frame);
            for (int i = 0; i < SCRUB_AUDIO_WINDOW; i++ ) {
                Mlt::Frame* f = activeProducer->get_frame();
                pushConsumer->push(*f);
                delete f;
            }

            activeProducer->seek(frame);
        }
    }

    void initializeConsumers() {
        pushConsumer.reset(new Mlt::PushConsumer(*profile, "sdl2_audio"));
        pullConsumer.reset(new Mlt::Consumer(*profile, "sdl2_audio"));
        pullConsumerConnection.reset(pullConsumer->listen("consumer-frame-show", m_self, (mlt_listener)mltOnConsumerFrameShow));
    }

    void cleanupConsumers() {
        if (pullConsumer && !pullConsumer->is_stopped()) {
            pullConsumer->stop();
        }

        if (pushConsumer && !pushConsumer->is_stopped()) {
            pushConsumer->stop();
        }

        pullConsumer.reset();
        pushConsumer.reset();
        pullConsumerConnection.reset();
    }

    KisCanvas2* activeCanvas() {
        return m_self->activeCanvas();
    }

    PlaybackMode activePlaybackMode() {
        KIS_ASSERT_RECOVER_RETURN_VALUE(activeCanvas(), PLAYBACK_PUSH);
        KIS_ASSERT_RECOVER_RETURN_VALUE(activeCanvas()->animationState(), PLAYBACK_PUSH);
        return activeCanvas()->animationState()->playbackState() == PlaybackState::PLAYING ? PLAYBACK_PULL : PLAYBACK_PUSH;
    }

    QSharedPointer<Mlt::Producer> activeProducer() {
        KIS_ASSERT_RECOVER_RETURN_VALUE(activeCanvas(), nullptr);
        KIS_ASSERT_RECOVER_RETURN_VALUE(canvasProducers.contains(activeCanvas()), nullptr);
        return canvasProducers[activeCanvas()];
    }

    bool dropFrames() const {
        return m_self->dropFrames();
    }

private:
    KisPlaybackEngineMLT* m_self;

public:
    QScopedPointer<Mlt::Repository> repository;
    QScopedPointer<Mlt::Profile> profile;

    QScopedPointer<Mlt::Consumer> pullConsumer;
    QScopedPointer<Mlt::Event> pullConsumerConnection;

    QScopedPointer<Mlt::PushConsumer> pushConsumer;

    QMap<KisCanvas2*, QSharedPointer<Mlt::Producer>> canvasProducers;

    QScopedPointer<KisSignalCompressorWithParam<int>> sigPushAudioCompressor;
    QScopedPointer<KisSignalCompressorWithParam<double>> sigSetPlaybackSpeed;

    double playbackSpeed;
    bool mute;

    FrameWaitingInterface frameWaitingInterface;
    FrameRenderingStats frameStats;
};

struct KisPlaybackEngineMLT::StopAndResume {
public:
    explicit StopAndResume(KisPlaybackEngineMLT::Private* p_d, bool requireFullRestart = false)
        : m_d(p_d)
    {
        KIS_ASSERT(p_d);

        {
            QMutexLocker l(&m_d->frameWaitingInterface.renderingControlMutex);
            m_d->frameWaitingInterface.renderingAllowed = false;
            m_d->frameWaitingInterface.renderingWaitCondition.wakeAll();
        }

        m_d->pushConsumer->stop();
        m_d->pushConsumer->purge();
        m_d->pullConsumer->stop();
        m_d->pullConsumer->purge();
        m_d->pullConsumer->disconnect_all_producers();

        if (requireFullRestart) {
            m_d->cleanupConsumers();
        }
    }

    ~StopAndResume() {
        KIS_ASSERT(m_d);
        if (!m_d->pushConsumer || !m_d->pullConsumer) {
            m_d->initializeConsumers();
        }

        if (m_d->activeCanvas()) {
            KisCanvasAnimationState* animationState = m_d->activeCanvas()->animationState();
            KIS_SAFE_ASSERT_RECOVER_RETURN(animationState);

            {
                QMutexLocker l(&m_d->frameWaitingInterface.renderingControlMutex);
                m_d->frameWaitingInterface.renderingAllowed = true;
                m_d->frameWaitingInterface.waitingForFrame = false;

                m_d->frameWaitingInterface.renderingWaitCondition.wakeAll();
            }

            m_d->frameStats.reset();

            {

                KisImageAnimationInterface* animInterface = m_d->activeCanvas()->image()->animationInterface();
                m_d->activeProducer()->set("start_frame", animInterface->activePlaybackRange().start());
                m_d->activeProducer()->set("end_frame", animInterface->activePlaybackRange().end());
                m_d->activeProducer()->set("speed", m_d->playbackSpeed);
                const int shouldLimit = m_d->activePlaybackMode() == PLAYBACK_PUSH ? 0 : 1;
                m_d->activeProducer()->set("limit_enabled", shouldLimit);
            }

            if (m_d->activePlaybackMode() == PLAYBACK_PUSH) {
                m_d->pushConsumer->set("volume", m_d->mute ? 0.0 : animationState->currentVolume());
                m_d->pushConsumer->start();
            } else {
                m_d->pullConsumer->connect_producer(*m_d->activeProducer());
                m_d->pullConsumer->set("volume", m_d->mute ? 0.0 : animationState->currentVolume());
                m_d->pullConsumer->set("real_time", m_d->dropFrames() ? 1 : 0);
                m_d->pullConsumer->start();
            }
        }
    }

private:
    Private* m_d;
};

KisPlaybackEngineMLT::KisPlaybackEngineMLT(QObject *parent)
    : KisPlaybackEngine(parent)
    , m_d( new Private(this))
{
    connect(this, &KisPlaybackEngineMLT::sigChangeActiveCanvasFrame, this, &KisPlaybackEngineMLT::throttledShowFrame, Qt::UniqueConnection);
}

KisPlaybackEngineMLT::~KisPlaybackEngineMLT()
{
}

void KisPlaybackEngineMLT::seek(int frameIndex, SeekOptionFlags flags)
{
    KIS_ASSERT(activeCanvas() && activeCanvas()->animationState());
    KisCanvasAnimationState* animationState = activeCanvas()->animationState();

    if (m_d->activePlaybackMode() == PLAYBACK_PUSH) {
        m_d->canvasProducers[activeCanvas()]->seek(frameIndex);

        if (flags & SEEK_PUSH_AUDIO) {

            m_d->sigPushAudioCompressor->start(frameIndex);
        }

        animationState->showFrame(frameIndex, (flags & SEEK_FINALIZE) > 0);
    }
}

void KisPlaybackEngineMLT::setupProducer(boost::optional<QFileInfo> file)
{
    if (!m_d->canvasProducers.contains(activeCanvas())) {
        connect(activeCanvas(), SIGNAL(destroyed(QObject*)), this, SLOT(canvasDestroyed(QObject*)));
    }

    m_d->canvasProducers[activeCanvas()] = QSharedPointer<Mlt::Producer>(new Mlt::Producer(*m_d->profile, "krita_play_chunk", "count"));

    if (file.has_value()) {
        QSharedPointer<Mlt::Producer> producer(

#ifdef Q_OS_ANDROID
            new Mlt::Producer(*m_d->profile,
                              "krita_play_chunk",
                              KisAndroidFileProxy::getFileFromContentUri(file->absoluteFilePath()).toUtf8().data()));
#else
        new Mlt::Producer(*m_d->profile, "krita_play_chunk", file->absoluteFilePath().toUtf8().data()));
#endif
        if (producer->is_valid()) {
            m_d->canvasProducers[activeCanvas()] = producer;
        } else {
            KIS_SAFE_ASSERT_RECOVER_NOOP(qEnvironmentVariableIsSet("MLT_REPOSITORY"));
            KIS_SAFE_ASSERT_RECOVER_NOOP(qEnvironmentVariableIsSet("MLT_PROFILES_PATH"));
            KIS_SAFE_ASSERT_RECOVER_NOOP(qEnvironmentVariableIsSet("MLT_PRESETS_PATH"));
            qDebug() << "Warning: Invalid MLT producer for file: " << ppVar(file->absoluteFilePath()) << " Falling back to audio-less playback.";
        }
    }

    KisImageAnimationInterface *animInterface = activeCanvas()->image()->animationInterface();
    QSharedPointer<Mlt::Producer> producer = m_d->canvasProducers[activeCanvas()];
    KIS_ASSERT(producer->is_valid());
    KIS_ASSERT(animInterface);

    producer->set("start_frame", animInterface->documentPlaybackRange().start());
    producer->set("end_frame", animInterface->documentPlaybackRange().end());
    producer->set("limit_enabled", false);
    producer->set("speed", m_d->playbackSpeed);
}

void KisPlaybackEngineMLT::setCanvas(KoCanvasBase *p_canvas)
{
    KisCanvas2* canvas = dynamic_cast<KisCanvas2*>(p_canvas);

    if (activeCanvas() == canvas) {
        return;
    }

    if (activeCanvas()) {
        KisCanvasAnimationState* animationState = activeCanvas()->animationState();

        if (animationState) {
            this->disconnect(animationState);
            animationState->disconnect(this);
        }

        auto image = activeCanvas()->image();
        if (image && image->animationInterface()) {
            this->disconnect(image->animationInterface());
            image->animationInterface()->disconnect(this);
        }
    }

    StopAndResume stopResume(m_d.data(), true);

    KisPlaybackEngine::setCanvas(p_canvas);

    if (activeCanvas()) {
        KisCanvasAnimationState* animationState = activeCanvas()->animationState();
        KIS_SAFE_ASSERT_RECOVER_RETURN(animationState);

        connect(animationState, &KisCanvasAnimationState::sigPlaybackStateChanged, this, [this](PlaybackState state){
            Q_UNUSED(state);
            StopAndResume callbackStopResume(m_d.data());
        });

        connect(animationState, &KisCanvasAnimationState::sigPlaybackMediaChanged, this, [this](){
            KisCanvasAnimationState* animationState = activeCanvas()->animationState();
            if (animationState) {
                setupProducer(animationState->mediaInfo());
            }
        });

        connect(animationState, &KisCanvasAnimationState::sigPlaybackSpeedChanged, this, [this](qreal value){
            m_d->sigSetPlaybackSpeed->start(value);
        });
        m_d->playbackSpeed = animationState->playbackSpeed();

        connect(animationState, &KisCanvasAnimationState::sigAudioLevelChanged, this, &KisPlaybackEngineMLT::setAudioVolume);

        auto image = activeCanvas()->image();
        KIS_SAFE_ASSERT_RECOVER_RETURN(image);

        connect(image->animationInterface(), &KisImageAnimationInterface::sigFramerateChanged, this, [this](){
            StopAndResume callbackStopResume(m_d.data());
            m_d->profile->set_frame_rate(activeCanvas()->image()->animationInterface()->framerate(), 1);

            KisCanvasAnimationState* animationState = activeCanvas()->animationState();
            if (animationState) {
                setupProducer(animationState->mediaInfo());
            }
        });

        m_d->profile->set_frame_rate(activeCanvas()->image()->animationInterface()->framerate(), 1);

        connect(image->animationInterface(), &KisImageAnimationInterface::sigPlaybackRangeChanged, this, [this](){
            QSharedPointer<Mlt::Producer> producer = m_d->canvasProducers[activeCanvas()];
            auto image = activeCanvas()->image();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);
            producer->set("start_frame", image->animationInterface()->activePlaybackRange().start());
            producer->set("end_frame", image->animationInterface()->activePlaybackRange().end());
        });

        setupProducer(animationState->mediaInfo());
    }

}

void KisPlaybackEngineMLT::unsetCanvas() {
    setCanvas(nullptr);
}

void KisPlaybackEngineMLT::canvasDestroyed(QObject *canvas)
{
    KIS_SAFE_ASSERT_RECOVER_RETURN(m_d->activeCanvas() != canvas);

    for (auto it = m_d->canvasProducers.begin(); it != m_d->canvasProducers.end(); ++it) {
        if (it.key() == canvas) {
            m_d->canvasProducers.erase(it);
            break;
        }
    }
}

void KisPlaybackEngineMLT::throttledShowFrame(const int frame)
{
    if (activeCanvas() && activeCanvas()->animationState() &&
            m_d->activePlaybackMode() == PLAYBACK_PULL ) {

        if (m_d->frameStats.lastRenderedFrame < 0) {
            m_d->frameStats.timeSinceLastFrame.start();
        } else {
            const int droppedFrames = qMax(0, frame - m_d->frameStats.lastRenderedFrame - 1);
            m_d->frameStats.averageFrameDuration(m_d->frameStats.timeSinceLastFrame.restart());
            m_d->frameStats.droppedFramesCount(droppedFrames);
        }
        m_d->frameStats.lastRenderedFrame = frame;

        activeCanvas()->animationState()->showFrame(frame);
    }

    {
        QMutexLocker l(&m_d->frameWaitingInterface.renderingControlMutex);
        m_d->frameWaitingInterface.waitingForFrame = false;
        m_d->frameWaitingInterface.renderingWaitCondition.wakeAll();
    }
}

void KisPlaybackEngineMLT::throttledSetSpeed(const double speed)
{
    StopAndResume stopResume(m_d.data(), false);
    m_d->playbackSpeed = speed;
}

void KisPlaybackEngineMLT::setAudioVolume(qreal volumeNormalized)
{
    if (m_d->mute) {
        m_d->pullConsumer->set("volume", 0.0);
        m_d->pushConsumer->set("volume", 0.0);
    } else {
        m_d->pullConsumer->set("volume", volumeNormalized);
        m_d->pushConsumer->set("volume", volumeNormalized);
    }
}

KisPlaybackEngineMLT::FrameWaitingInterface *KisPlaybackEngineMLT::frameWaitingInterface()
{
    return &m_d->frameWaitingInterface;
}

void KisPlaybackEngineMLT::setDropFramesMode(bool value)
{
    StopAndResume r(m_d.data(), false);

    KisPlaybackEngine::setDropFramesMode(value);
}

void KisPlaybackEngineMLT::setMute(bool val)
{
    KIS_SAFE_ASSERT_RECOVER_RETURN(activeCanvas() && activeCanvas()->animationState());
    KisCanvasAnimationState* animationState = activeCanvas()->animationState();

    qreal currentVolume = animationState->currentVolume();
    m_d->mute = val;
    setAudioVolume(currentVolume);
}

bool KisPlaybackEngineMLT::isMute()
{
    return m_d->mute;
}

KisPlaybackEngine::PlaybackStats KisPlaybackEngineMLT::playbackStatistics() const
{
    KisPlaybackEngine::PlaybackStats stats;

    if (activeCanvas() && activeCanvas()->animationState() &&
        m_d->activePlaybackMode() == PLAYBACK_PULL ) {

        const int droppedFrames = m_d->frameStats.droppedFramesCount.rollingSum();
        const int totalFrames =
            m_d->frameStats.droppedFramesCount.rollingCount() +
            droppedFrames;

        stats.droppedFramesPortion = qreal(droppedFrames) / totalFrames;
        stats.expectedFps = qreal(activeCanvas()->image()->animationInterface()->framerate()) * m_d->playbackSpeed;

        const qreal avgTimePerFrame = m_d->frameStats.averageFrameDuration.rollingMeanSafe();
        stats.realFps = !qFuzzyIsNull(avgTimePerFrame) ? 1000.0 / avgTimePerFrame : 0.0;

    }

    return stats;
}

