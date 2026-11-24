/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_STROKE_STRATEGY_H
#define __KIS_STROKE_STRATEGY_H

#include <QString>
#include "kis_types.h"
#include "kundo2magicstring.h"
#include "kritaimage_export.h"
#include "KisLodPreferences.h"

class KisStrokeJobStrategy;
class KisStrokeJobData;
class KisStrokesQueueMutatedJobInterface;

class KRITAIMAGE_EXPORT KisStrokeStrategy
{
public:
    KisStrokeStrategy(const QLatin1String &id, const KUndo2MagicString &name = KUndo2MagicString());
    virtual ~KisStrokeStrategy();

    virtual void notifyUserStartedStroke();

    virtual void notifyUserEndedStroke();

    virtual KisStrokeJobStrategy* createInitStrategy();
    virtual KisStrokeJobStrategy* createFinishStrategy();
    virtual KisStrokeJobStrategy* createCancelStrategy();
    virtual KisStrokeJobStrategy* createDabStrategy();
    virtual KisStrokeJobStrategy* createSuspendStrategy();
    virtual KisStrokeJobStrategy* createResumeStrategy();

    virtual KisStrokeJobData* createInitData();
    virtual KisStrokeJobData* createFinishData();
    virtual KisStrokeJobData* createCancelData();
    virtual KisStrokeJobData* createSuspendData();
    virtual KisStrokeJobData* createResumeData();

    virtual KisStrokeStrategy* createLodClone(int levelOfDetail);

    virtual void tryCancelCurrentStrokeJobAsync();

    bool isExclusive() const;
    bool supportsWrapAroundMode() const;

    bool clearsRedoOnStart() const;

    bool requestsOtherStrokesToEnd() const;

    bool canForgetAboutMe() const;

    bool isAsynchronouslyCancellable() const;

    bool needsExplicitCancel() const;

    qreal balancingRatioOverride() const;

    QString id() const;
    KUndo2MagicString name() const;

    KisLodPreferences currentLodPreferences() const;

    void setMutatedJobsInterface(KisStrokesQueueMutatedJobInterface *mutatedJobsInterface, KisStrokeId strokeId);

    bool forceLodModeIfPossible() const;
    void setForceLodModeIfPossible(bool forceLodModeIfPossible);

protected:
    friend class KisMutatableDabStrategy;

    void addMutatedJobs(const QVector<KisStrokeJobData*> list);

    void addMutatedJob(KisStrokeJobData *data);

    void setExclusive(bool value);
    void setSupportsWrapAroundMode(bool value);
    void setClearsRedoOnStart(bool value);
    void setRequestsOtherStrokesToEnd(bool value);
    void setCanForgetAboutMe(bool value);
    void setAsynchronouslyCancellable(bool value);
    void setNeedsExplicitCancel(bool value);

    void setBalancingRatioOverride(qreal value);

protected:
    KisStrokeStrategy(const KisStrokeStrategy &rhs);

private:
    bool m_exclusive;
    bool m_supportsWrapAroundMode;

    bool m_clearsRedoOnStart;
    bool m_requestsOtherStrokesToEnd;
    bool m_canForgetAboutMe;
    bool m_asynchronouslyCancellable;
    bool m_needsExplicitCancel;
    bool m_forceLodModeIfPossible;
    qreal m_balancingRatioOverride;

    QLatin1String m_id;
    KUndo2MagicString m_name;

    KisStrokeId m_strokeId;
    KisStrokesQueueMutatedJobInterface *m_mutatedJobsInterface;
};

#endif
