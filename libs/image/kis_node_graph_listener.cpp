/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_node_graph_listener.h"

#include "kis_time_span.h"
#include <QRect>
#include <QtGlobal>

struct Q_DECL_HIDDEN KisNodeGraphListener::Private
{
    Private() : sequenceNumber(0) {}
    int sequenceNumber;
};

KisNodeGraphListener::KisNodeGraphListener()
    : m_d(new Private())
{
}

KisNodeGraphListener::~KisNodeGraphListener()
{
}

void KisNodeGraphListener::aboutToAddANode(KisNode * , int )
{
    m_d->sequenceNumber++;
}

void KisNodeGraphListener::nodeHasBeenAdded(KisNode * , int , KisNodeAdditionFlags )
{
    m_d->sequenceNumber++;
}

void KisNodeGraphListener::aboutToRemoveANode(KisNode * , int )
{
    m_d->sequenceNumber++;
}

void KisNodeGraphListener::nodeHasBeenRemoved(KisNode * , int )
{
    m_d->sequenceNumber++;
}

void KisNodeGraphListener::aboutToMoveNode(KisNode * , int , int )
{
    m_d->sequenceNumber++;
}

void KisNodeGraphListener::nodeHasBeenMoved(KisNode * , int , int )
{
    m_d->sequenceNumber++;
}

int KisNodeGraphListener::graphSequenceNumber() const
{
    return m_d->sequenceNumber;
}

void KisNodeGraphListener::keyframeChannelHasBeenAdded(KisNode * , KisKeyframeChannel * )
{

}

void KisNodeGraphListener::keyframeChannelAboutToBeRemoved(KisNode * , KisKeyframeChannel * )
{

}

void KisNodeGraphListener::nodeChanged(KisNode * )
{
}

void KisNodeGraphListener::nodeCollapsedChanged(KisNode * )
{
}

void KisNodeGraphListener::invalidateAllFrames()
{
}

void KisNodeGraphListener::notifySelectionChanged()
{
}

void KisNodeGraphListener::requestProjectionUpdate(KisNode * , const QVector<QRect> & , KisProjectionUpdateFlags )
{
}

void KisNodeGraphListener::invalidateFrames(const KisTimeSpan &range, const QRect &rect)
{
    Q_UNUSED(range);
    Q_UNUSED(rect);
}

void KisNodeGraphListener::requestTimeSwitch(int time)
{
    Q_UNUSED(time);
}

KisNode *KisNodeGraphListener::graphOverlayNode() const
{
    return 0;
}
