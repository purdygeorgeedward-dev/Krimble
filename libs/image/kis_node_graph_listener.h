/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_NODE_GRAPH_LISTENER_H_
#define KIS_NODE_GRAPH_LISTENER_H_

#include "kritaimage_export.h"

#include <QScopedPointer>
#include <KisProjectionUpdateFlags.h>
#include <KisNodeAdditionFlags.h>

class KisTimeSpan;
class KisNode;
class QRect;
class KisKeyframeChannel;

class KRITAIMAGE_EXPORT KisNodeGraphListener
{

public:
    KisNodeGraphListener();

    virtual ~KisNodeGraphListener();

    virtual void aboutToAddANode(KisNode *parent, int index);

    virtual void nodeHasBeenAdded(KisNode *parent, int index, KisNodeAdditionFlags flags);

    virtual void aboutToRemoveANode(KisNode *parent, int index);

    virtual void nodeHasBeenRemoved(KisNode *parent, int index);

    virtual void aboutToMoveNode(KisNode * node, int oldIndex, int newIndex);

    virtual void nodeHasBeenMoved(KisNode * node, int oldIndex, int newIndex);

    virtual void nodeChanged(KisNode * node);

    virtual void nodeCollapsedChanged(KisNode * node);

    virtual void invalidateAllFrames();

    virtual void notifySelectionChanged();

    virtual void requestProjectionUpdate(KisNode * node, const QVector<QRect> &rects, KisProjectionUpdateFlags flags);

    virtual void invalidateFrames(const KisTimeSpan &range, const QRect &rect);

    virtual void requestTimeSwitch(int time);

    virtual KisNode* graphOverlayNode() const;

     int graphSequenceNumber() const;

     virtual void keyframeChannelHasBeenAdded(KisNode *node, KisKeyframeChannel *channel);
     virtual void keyframeChannelAboutToBeRemoved(KisNode *node, KisKeyframeChannel *channel);

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
