/*
 *  SPDX-FileCopyrightText: 2019 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISSAFENODEPROJECTIONSTORE_H
#define KISSAFENODEPROJECTIONSTORE_H

#include <QObject>

#include <QScopedPointer>

#include "kritaimage_export.h"
#include "kis_shared.h"
#include "kis_types.h"
#include "kis_image_interfaces.h"

struct StoreImplementationInterface;

class KRITAIMAGE_EXPORT KisSafeNodeProjectionStoreBase : public QObject, public KisShared
{
    Q_OBJECT

public:
    KisSafeNodeProjectionStoreBase(const KisSafeNodeProjectionStoreBase &rhs);
    ~KisSafeNodeProjectionStoreBase();

    void releaseDevice();

    void setImage(KisImageWSP image);

Q_SIGNALS:
    void internalInitiateProjectionsCleanup();

private Q_SLOTS:
    void slotInitiateProjectionsCleanup();

protected:
    KisSafeNodeProjectionStoreBase(StoreImplementationInterface *storeImpl);

private:
    void discardCaches();

    friend class KisRecycleProjectionsJob;
    void recycleProjectionsInSafety();

protected:
    struct Private;
    QScopedPointer<Private> m_d;
};

class KRITAIMAGE_EXPORT KisSafeNodeProjectionStore : public KisSafeNodeProjectionStoreBase
{
public:
    KisSafeNodeProjectionStore();
    KisSafeNodeProjectionStore(const KisSafeNodeProjectionStore &rhs);

    KisPaintDeviceSP getDeviceLazy(KisPaintDeviceSP prototype);
};

class KRITAIMAGE_EXPORT KisSafeSelectionNodeProjectionStore : public KisSafeNodeProjectionStoreBase
{
public:
    KisSafeSelectionNodeProjectionStore();
    KisSafeSelectionNodeProjectionStore(const KisSafeSelectionNodeProjectionStore &rhs);

    KisSelectionSP getDeviceLazy(KisSelectionSP prototype);
};

typedef KisSharedPtr<KisSafeNodeProjectionStoreBase> KisSafeNodeProjectionStoreBaseSP;
typedef KisWeakSharedPtr<KisSafeNodeProjectionStoreBase> KisSafeNodeProjectionStoreBaseWSP;

typedef KisSharedPtr<KisSafeNodeProjectionStore> KisSafeNodeProjectionStoreSP;
typedef KisWeakSharedPtr<KisSafeNodeProjectionStore> KisSafeNodeProjectionStoreWSP;

typedef KisSharedPtr<KisSafeSelectionNodeProjectionStore> KisSafeSelectionNodeProjectionStoreSP;
typedef KisWeakSharedPtr<KisSafeSelectionNodeProjectionStore> KisSafeSelectionNodeProjectionStoreWSP;

#endif
