/*
 *  SPDX-FileCopyrightText: 2019 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISRECYCLEPROJECTIONSJOB_H
#define KISRECYCLEPROJECTIONSJOB_H

#include "kis_types.h"
#include "kis_spontaneous_job.h"

class QMutex;
class KisSafeNodeProjectionStoreBase;
typedef KisWeakSharedPtr<KisSafeNodeProjectionStoreBase> KisSafeNodeProjectionStoreBaseWSP;

class KRITAIMAGE_EXPORT KisRecycleProjectionsJob : public KisSpontaneousJob
{
public:
    KisRecycleProjectionsJob(KisSafeNodeProjectionStoreBaseWSP projectionStore);

    bool overrides(const KisSpontaneousJob *otherJob) override;
    void run() override;
    int levelOfDetail() const override;

    QString debugName() const override;

private:
    KisSafeNodeProjectionStoreBaseWSP m_projectionStore;
};

#endif
