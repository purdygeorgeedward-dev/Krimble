/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISFAKERUNNABLESTROKEJOBSEXECUTOR_H
#define KISFAKERUNNABLESTROKEJOBSEXECUTOR_H

#include "KisRunnableStrokeJobsInterface.h"

class KRITAIMAGE_EXPORT KisFakeRunnableStrokeJobsExecutor : public KisRunnableStrokeJobsInterface
{
public:

    enum Flag {
        None = 0x0,
        AllowBarrierJobs = 0x1
    };
    Q_DECLARE_FLAGS(Flags, Flag)

public:
    KisFakeRunnableStrokeJobsExecutor();
    KisFakeRunnableStrokeJobsExecutor(Flags flags);

    void addRunnableJobs(const QVector<KisRunnableStrokeJobDataBase*> &list) override;

private:
    Flags m_flags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisFakeRunnableStrokeJobsExecutor::Flags)

#endif
