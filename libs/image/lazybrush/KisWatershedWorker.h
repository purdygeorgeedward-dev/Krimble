/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISWATERSHEDWORKER_H
#define KISWATERSHEDWORKER_H

#include <QScopedPointer>

#include "kis_types.h"
#include "kritaimage_export.h"

class KoColor;

class KRITAIMAGE_EXPORT KisWatershedWorker
{
public:
    KisWatershedWorker(KisPaintDeviceSP heightMap,
                       KisPaintDeviceSP dst,
                       const QRect &boundingRect,
                       KoUpdater *progress = 0);
    ~KisWatershedWorker();

    void addKeyStroke(KisPaintDeviceSP dev, const KoColor &color);

    void run(qreal cleanUpAmount = 0.0);

    int testingGroupPositiveEdge(qint32 group, quint8 level);
    int testingGroupNegativeEdge(qint32 group, quint8 level);
    int testingGroupForeignEdge(qint32 group, quint8 level);
    int testingGroupAllyEdge(qint32 group, quint8 level);
    int testingGroupConflicts(qint32 group, quint8 level, qint32 withGroup);

    void testingTryRemoveGroup(qint32 group, quint8 level);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
