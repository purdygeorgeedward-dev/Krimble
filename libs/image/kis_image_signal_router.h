/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_IMAGE_SIGNAL_ROUTER_H
#define __KIS_IMAGE_SIGNAL_ROUTER_H

#include <QObject>
#include "KisImageSignals.h"
#include "KisNodeAdditionFlags.h"

#include <kis_node.h>
class KoColorSpace;
class KoColorProfile;

class KRITAIMAGE_EXPORT KisImageSignalRouter : public QObject
{
    Q_OBJECT

public:
    KisImageSignalRouter(KisImageWSP image);
    ~KisImageSignalRouter() override;

    void emitNotification(KisImageSignalType type);
    void emitNotifications(KisImageSignalVector notifications);

    void emitNodeChanged(KisNodeSP node);
    void emitNodeHasBeenAdded(KisNode *parent, int index, KisNodeAdditionFlags flags);
    void emitAboutToRemoveANode(KisNode *parent, int index);

    void emitRequestLodPlanesSyncBlocked(bool value);
    void emitNotifyBatchUpdateStarted();
    void emitNotifyBatchUpdateEnded();

public Q_SLOTS:
    void emitImageModifiedNotification();

private Q_SLOTS:
    void slotNotification(KisImageSignalType type);

Q_SIGNALS:

    void sigNotification(KisImageSignalType type);

    void sigRequestLodPlanesSyncBlocked(bool value);

    void sigNotifyBatchUpdateStarted();

    void sigNotifyBatchUpdateEnded();

    void sigImageModified();
    void sigImageModifiedWithoutUndo();

    void sigSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint);
    void sigProfileChanged(const KoColorProfile *  profile);
    void sigColorSpaceChanged(const KoColorSpace*  cs);
    void sigResolutionChanged(double xRes, double yRes);
    void sigRequestNodeReselection(KisNodeSP activeNode, const KisNodeList &selectedNodes);

    void sigNodeChanged(KisNodeSP node);
    void sigNodeAddedAsync(KisNodeSP node, KisNodeAdditionFlags flags);
    void sigRemoveNodeAsync(KisNodeSP node);
    void sigLayersChangedAsync();

private:
    KisImageWSP m_image;
};

#endif
