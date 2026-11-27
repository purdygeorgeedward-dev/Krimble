/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "KisIdleTaskStrokeStrategy.h"

KisIdleTaskStrokeStrategy::KisIdleTaskStrokeStrategy(const QLatin1String &id, const KUndo2MagicString &name)
    : KisRunnableBasedStrokeStrategy(id, name)
{
    enableJob(KisSimpleStrokeStrategy::JOB_INIT, true, KisStrokeJobData::BARRIER, KisStrokeJobData::EXCLUSIVE);
    enableJob(KisSimpleStrokeStrategy::JOB_DOSTROKE);
    enableJob(KisSimpleStrokeStrategy::JOB_FINISH, true, KisStrokeJobData::SEQUENTIAL, KisStrokeJobData::EXCLUSIVE);
    enableJob(KisSimpleStrokeStrategy::JOB_CANCEL, true, KisStrokeJobData::SEQUENTIAL, KisStrokeJobData::EXCLUSIVE);

    setRequestsOtherStrokesToEnd(false);
    setClearsRedoOnStart(false);
    setCanForgetAboutMe(true);
}

KisIdleTaskStrokeStrategy::~KisIdleTaskStrokeStrategy() = default;

KisStrokeStrategy *KisIdleTaskStrokeStrategy::createLodClone(int levelOfDetail)
{
    Q_UNUSED(levelOfDetail);
    return new KisSimpleStrokeStrategy(QLatin1String("KisIdleTaskStrokeStrategy_FakeLodN"));
}

void KisIdleTaskStrokeStrategy::initStrokeCallback()
{
    m_idleStrokeTime.start();
}

void KisIdleTaskStrokeStrategy::finishStrokeCallback()
{
    Q_EMIT sigIdleTaskFinished();

    const qint64 elapsedTime = m_idleStrokeTime.elapsed();

    if (elapsedTime > preferredIdleTaskMaximumTime()) {
        qWarning() << "WARNING: idle task consumed more than" <<
                      preferredIdleTaskMaximumTime() <<
                      "ms, it can cause visible distractions to the user";
        qWarning() << "WARNING: time consumed in" << id() <<
                      "is" << elapsedTime << "ms";
    }
}

QWeakPointer<boost::none_t> KisIdleTaskStrokeStrategy::idleTaskCookie()
{
    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(!m_idleTaskCookie, m_idleTaskCookie);
    m_idleTaskCookie.reset(new boost::none_t(boost::none));
    return m_idleTaskCookie;
}

int KisIdleTaskStrokeStrategy::preferredIdleTaskMaximumTime()
{

    return 200;
}

int KisIdleTaskStrokeStrategy::preferredIdleWatcherInterval()
{
    return 50;
}

