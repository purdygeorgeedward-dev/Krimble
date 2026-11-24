/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_NODE_H
#define _KIS_NODE_H

#include "kis_types.h"

#include "kis_base_node.h"
#include "KisNodeAdditionFlags.h"
#include <KisQStringListFwd.h>

#include "kritaimage_export.h"

#include <QVector>
#include <KisRegion.h>

class QRect;
class KoProperties;

class KisNodeVisitor;
class KisNodeGraphListener;
class KisNodeProgressProxy;
class KisBusyProgressIndicator;
class KisAbstractProjectionPlane;
class KisProjectionLeaf;
class KisKeyframeChannel;
class KisTimeSpan;
class KisUndoAdapter;
struct KisFrameChangeUpdateRecipe;

class KRITAIMAGE_EXPORT KisNode : public KisBaseNode
{
    friend class KisFilterMaskTest;
    Q_OBJECT

public:
    enum PositionToFilthy {
        N_ABOVE_FILTHY = 0x08,
        N_FILTHY_PROJECTION = 0x20,
        N_FILTHY = 0x40,
        N_BELOW_FILTHY = 0x80
    };

    KisNode(KisImageWSP image);

    KisNode(const KisNode & rhs);

    ~KisNode() override;

    virtual KisNodeSP clone() const = 0;

    bool accept(KisNodeVisitor &v) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    virtual bool allowAsChild(KisNodeSP) const = 0;

    virtual void setDirty();

    void setDirty(const QRect & rect);

    virtual void setDirty(const QVector<QRect> &rects);

    void setDirty(const KisRegion &region);

    void setDirtyDontResetAnimationCache();

    void setDirtyDontResetAnimationCache(const QRect &rect);

    void setDirtyDontResetAnimationCache(const QVector<QRect> &rects);

    void invalidateFrames(const KisTimeSpan &range, const QRect &rect);

    virtual void handleKeyframeChannelFrameChange(const KisKeyframeChannel *channel, int time);
    virtual void handleKeyframeChannelFrameAdded(const KisKeyframeChannel *channel, int time);
    void handleKeyframeChannelFrameAboutToBeRemoved(const KisKeyframeChannel *channel, int time);
    void handleKeyframeChannelFrameHasBeenRemoved(const KisKeyframeChannel *channel, int time);

    void requestTimeSwitch(int time);

    virtual KisAbstractProjectionPlaneSP projectionPlane() const;

    virtual void syncLodCache();
    virtual KisPaintDeviceList getLodCapableDevices() const;

    virtual KisProjectionLeafSP projectionLeaf() const;

    void setImage(KisImageWSP newImage) override;

protected:

    virtual QRect changeRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const;

    virtual QRect needRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const;

    virtual QRect accessRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const;

    virtual void childNodeChanged(KisNodeSP changedChildNode);

public:

    int graphSequenceNumber() const;

    KisNodeGraphListener * graphListener() const;

    void setGraphListener(KisNodeGraphListener * graphListener);

    KisNodeSP parent() const;

    KisNodeSP firstChild() const;

    KisNodeSP lastChild() const;

    KisNodeSP prevSibling() const;

    KisNodeSP nextSibling() const;

    quint32 childCount() const;

    KisNodeSP at(quint32 index) const;

    int index(const KisNodeSP node) const;

    QList<KisNodeSP> childNodes(const QStringList & nodeTypes, const KoProperties & properties) const;

Q_SIGNALS:
    void sigNodeChangedInternal();

public:

    KisNodeProgressProxy* nodeProgressProxy() const;

    KisBusyProgressIndicator* busyProgressIndicator() const;

private:

    void createNodeProgressProxy();

protected:
    KisBaseNodeSP parentCallback() const override;
    void notifyParentVisibilityChanged(bool value) override;
    void baseNodeChangedCallback() override;
    void baseNodeInvalidateAllFramesCallback() override;
    void baseNodeCollapsedChangedCallback() override;

protected:
    void addKeyframeChannel(KisKeyframeChannel* channel) override;
    virtual KisFrameChangeUpdateRecipe handleKeyframeChannelFrameAboutToBeRemovedImpl(const KisKeyframeChannel *channel, int time);

private:

    friend class KisNodeFacade;
    friend class KisNodeTest;
    friend class KisLayer;
    void setParent(KisNodeWSP parent);

    bool add(KisNodeSP newNode, KisNodeSP aboveThis, KisNodeAdditionFlags flags = KisNodeAdditionFlag::None);

    bool remove(quint32 index);

    bool remove(KisNodeSP node);

    KisNodeSP prevChildImpl(KisNodeSP child);
    KisNodeSP nextChildImpl(KisNodeSP child);

private:

    struct Private;
    Private * const m_d;

};

#ifndef Q_DECLARE_KISNODE
#define  Q_DECLARE_KISNODE
Q_DECLARE_METATYPE(KisNodeSP)
Q_DECLARE_METATYPE(KisNodeWSP)
#endif
#endif
