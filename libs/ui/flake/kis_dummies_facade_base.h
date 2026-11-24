/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_DUMMIES_FACADE_BASE_H
#define __KIS_DUMMIES_FACADE_BASE_H

#include <QObject>

#include "kis_types.h"
#include "kritaui_export.h"
#include <KisNodeAdditionFlags.h>

#include <kis_node.h>
class KisNodeDummy;

class KRITAUI_EXPORT KisDummiesFacadeBase : public QObject
{
    Q_OBJECT

public:
    KisDummiesFacadeBase(QObject *parent = 0);
    ~KisDummiesFacadeBase() override;

    void setImage(KisImageWSP image);
    virtual void setImage(KisImageWSP image, KisNodeSP activeNode);

    virtual bool hasDummyForNode(KisNodeSP node) const = 0;
    virtual KisNodeDummy* dummyForNode(KisNodeSP node) const = 0;

    virtual KisNodeDummy* rootDummy() const = 0;
    virtual int dummiesCount() const = 0;

    KisNodeSP lastActivatedNode() const;
    void setLastActivatedNode(KisNodeSP node);

protected:
    KisImageWSP image() const;

    virtual void addNodeImpl(KisNodeSP node, KisNodeSP parent, KisNodeSP aboveThis) = 0;
    virtual void removeNodeImpl(KisNodeSP node) = 0;

Q_SIGNALS:

    void sigBeginInsertDummy(KisNodeDummy *parent, int index, const QString &metaObjectType);
    void sigEndInsertDummy(KisNodeDummy *dummy);

    void sigBeginRemoveDummy(KisNodeDummy *dummy);
    void sigEndRemoveDummy();

    void sigDummyChanged(KisNodeDummy *dummy);

    void sigActivateNode(KisNodeSP node);

private Q_SLOTS:
    void slotLayersChanged();
    void slotNodeChanged(KisNodeSP node);

    void slotNodeActivationRequested(KisNodeSP node, KisNodeAdditionFlags flags);

    void slotNodeAdded(KisNodeSP node, KisNodeAdditionFlags flags);
    void slotRemoveNode(KisNodeSP node);

    void slotContinueAddNode(KisNodeSP node, KisNodeSP parent, KisNodeSP aboveThis);
    void slotContinueRemoveNode(KisNodeSP node);

private:
    static KisNodeSP findFirstLayer(KisNodeSP root);

private:
    struct Private;
    Private * const m_d;
};

#endif
