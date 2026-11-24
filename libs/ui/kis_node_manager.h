/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_NODE_MANAGER
#define KIS_NODE_MANAGER

#include <QObject>
#include <QList>
#include <QAction>

#include "kis_types.h"
#include "kis_base_node.h"
#include "kis_image.h"
#include <kritaui_export.h>

#include <kis_node.h>
#include <kis_selection.h>
class KisKActionCollection;

class KoCompositeOp;
class KoColorSpace;
class KUndo2MagicString;

class KisFilterStrategy;
class KisViewManager;
class KisActionManager;
class KisView;
class KisNodeSelectionAdapter;
class KisNodeInsertionAdapter;
class KisNodeDisplayModeAdapter;
class KisNodeJugglerCompressed;
class KoProperties;
class KisProcessingApplicator;

class KRITAUI_EXPORT KisNodeManager : public QObject
{

    Q_OBJECT

public:

    KisNodeManager(KisViewManager * view);
    ~KisNodeManager() override;

    void setView(QPointer<KisView>imageView);

Q_SIGNALS:

    void sigNodeActivated(KisNodeSP node);

    void sigUiNeedChangeActiveNode(KisNodeSP node);

    void sigUiNeedChangeSelectedNodes(const QList<KisNodeSP> &nodes);

public:

    void setup(KisKActionCollection * collection, KisActionManager* actionManager);
    void updateGUI();

    KisNodeSP activeNode();

    KisLayerSP activeLayer();

    KisPaintDeviceSP activePaintDevice();

    const KoColorSpace* activeColorSpace();

    void setNodeName(KisNodeSP node, const QString &name);

    void setNodeOpacity(KisNodeSP node, qint32 opacity);

    void setNodeCompositeOp(KisNodeSP node, const KoCompositeOp* compositeOp);

    KisNodeList selectedNodes();

    KisNodeSelectionAdapter* nodeSelectionAdapter() const;
    KisNodeInsertionAdapter* nodeInsertionAdapter() const;
    KisNodeDisplayModeAdapter* nodeDisplayModeAdapter() const;

    static bool isNodeHidden(KisNodeSP node, bool isGlobalSelectionHidden);

    bool trySetNodeProperties(KisNodeSP node, KisImageSP image, KisBaseNode::PropertyList properties) const;

    bool canModifyLayers(KisNodeList nodes, bool showWarning = true);
    bool canModifyLayer(KisNodeSP node, bool showWarning = true);

    bool canMoveLayers(KisNodeList nodes, bool showWarning = true);
    bool canMoveLayer(KisNodeSP node, bool showWarning = true);

public Q_SLOTS:

    void slotNonUiActivatedNode(KisNodeSP node);

    void slotUiActivatedNode(KisNodeSP node);

    void addNodesDirect(KisNodeList nodes, KisNodeSP parent, KisNodeSP aboveThis);

    void moveNodesDirect(KisNodeList nodes, KisNodeSP parent, KisNodeSP aboveThis);

    void copyNodesDirect(KisNodeList nodes, KisNodeSP parent, KisNodeSP aboveThis);

    void createFromVisible();

    void slotPinToTimeline(bool value);

    void toggleIsolateActiveNode();
    void setIsolateActiveLayerMode(bool checked);
    void setIsolateActiveGroupMode(bool checked);

    void changeIsolationMode(bool isolateActiveLayer, bool isolateActiveGroup);
    void changeIsolationRoot(KisNodeSP isolationRoot);

    void handleExternalIsolationChange();
    void reinitializeIsolationActionGroup();

    void moveNodeAt(KisNodeSP node, KisNodeSP parent, int index);
    KisNodeSP createNode(const QString& nodeType, bool quiet = false, KisPaintDeviceSP copyFrom = 0);
    void convertNode(const QString &nodeType);
    void createReferenceImage(bool fromLayer);
    void createReferenceImageFromLayer();
    void createReferenceImageFromVisible();
    void nodesUpdated();
    void nodeProperties(KisNodeSP node);
    void nodePropertiesIgnoreSelection(KisNodeSP node);
    void changeCloneSource();
    void nodeOpacityChanged(qreal opacity);
    void nodeCompositeOpChanged(const KoCompositeOp* op);
    void duplicateActiveNode();
    void removeNode();
    void mirrorNodeX();
    void mirrorNodeY();
    void mirrorAllNodesX();
    void mirrorAllNodesY();

    void mirrorNode(KisNodeSP node, const KUndo2MagicString& commandName, Qt::Orientation orientation, KisSelectionSP selection);
    void mirrorNodes(KisNodeList nodes, const KUndo2MagicString& commandName, Qt::Orientation orientation, KisSelectionSP selection);

    void activateNextNode(bool siblingsOnly = false);
    void activateNextSiblingNode();
    void activatePreviousNode(bool siblingsOnly = false);
    void activatePreviousSiblingNode();
    void switchToPreviouslyActiveNode();

    void raiseNode();

    void lowerNode();

    void saveNodeAsImage();
    void saveVectorLayerAsImage();

    void slotSplitAlphaIntoMask();
    void slotSplitAlphaWrite();
    void slotSplitAlphaSaveMerged();

    void toggleLock();
    void toggleVisibility();
    void toggleAlphaLock();
    void toggleInheritAlpha();

    void colorOverlayMaskProperties(KisNodeSP node);

    void slotSetSelectedNodes(const KisNodeList &nodes);

    void slotImageRequestNodeReselection(KisNodeSP activeNode, const KisNodeList &selectedNodes);

    void cutLayersToClipboard();
    void copyLayersToClipboard();
    void pasteLayersFromClipboard(bool changeOffset = false, QPointF offset = QPointF(), KisProcessingApplicator *applicator = nullptr);

    void createQuickGroup();
    void createQuickClippingGroup();
    void quickUngroup();

    void selectAllNodes();
    void selectVisibleNodes();
    void selectLockedNodes();
    void selectInvisibleNodes();
    void selectUnlockedNodes();

private Q_SLOTS:

    friend class KisNodeActivationActionCreatorVisitor;
    void slotUiActivateNode();

public:
    void removeSingleNode(KisNodeSP node);
    KisLayerSP createPaintLayer();

private:
    qint32 convertOpacityToInt(qreal opacity);
    void removeSelectedNodes(KisNodeList selectedNodes);
    void slotSomethingActivatedNodeImpl(KisNodeSP node);
    bool createQuickGroupImpl(KisNodeJugglerCompressed *juggler,
                              const QString &overrideGroupName,
                              KisNodeSP *newGroup,
                              KisNodeSP *newLastChild);
    void selectLayersImpl(const KoProperties &props, const KoProperties &invertedProps);

    struct Private;
    Private * const m_d;
};

#endif
