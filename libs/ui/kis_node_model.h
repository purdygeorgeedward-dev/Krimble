/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_NODE_MODEL
#define KIS_NODE_MODEL

#include "kritaui_export.h"
#include <kis_types.h>
#include <QAbstractItemModel>
#include <QList>
#include <QString>
#include <QVariant>

#include <KisSelectionTags.h>

#include <kis_node.h>
class KisDummiesFacadeBase;
class KisNodeDummy;
class KisShapeController;
class KisModelIndexConverterBase;
class KisNodeSelectionAdapter;
class KisNodeInsertionAdapter;
class KisSelectionActionsAdapter;
class KisNodeDisplayModeAdapter;
class KisNodeManager;
class KisIdleTasksManager;

class KRITAUI_EXPORT KisNodeModel : public QAbstractItemModel
{

    Q_OBJECT

public:
    enum ItemDataRole
    {
        ActiveRole = Qt::UserRole + 1,

        PropertiesRole,

        AspectRatioRole,

        ProgressRole,

        AlternateActiveRole,

        ShouldGrayOutRole,

        ColorLabelIndexRole,

        DropEnabled,

        SelectOpaqueRole,

        DropReasonRole,

        IsAnimatedRole,

        InfoTextRole,

        FilterMaskColorRole,

        FilterMaskPropertiesRole,

        LayerColorOverlayColorRole,

        LayerColorOverlayPropertiesRole,

        ReservedRole = Qt::UserRole + 99,

        BeginThumbnailRole
    };

public:

    explicit KisNodeModel(QObject * parent, int clonedColumns = 0);
    ~KisNodeModel() override;

    void setDummiesFacade(KisDummiesFacadeBase *dummiesFacade,
                          KisImageWSP image,
                          KisShapeController *shapeController,
                          KisSelectionActionsAdapter *selectionActionsAdapter,
                          KisNodeManager *nodeManager);
    void setIdleTaskManager(KisIdleTasksManager *idleTasksManager);
    KisNodeSP nodeFromIndex(const QModelIndex &index) const;
    QModelIndex indexFromNode(KisNodeSP node) const;

    bool showGlobalSelection() const;
    void setPreferredThumnalSize(int preferredSize) const;

public Q_SLOTS:
    void setShowGlobalSelection(bool value);

public:

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList & indexes) const override;
    bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    bool hasDummiesFacade();

    static bool belongsToIsolatedGroup(KisImageSP image, KisNodeSP node, KisDummiesFacadeBase *dummiesFacade);

Q_SIGNALS:

    void toggleIsolateActiveNode();
    void sigBeforeBeginRemoveRows(const QModelIndex &parent, int start, int end);

protected Q_SLOTS:
    void slotBeginInsertDummy(KisNodeDummy *parent, int index, const QString &metaObjectType);
    void slotEndInsertDummy(KisNodeDummy *dummy);
    void slotBeginRemoveDummy(KisNodeDummy *dummy);
    void slotEndRemoveDummy();
    void slotDummyChanged(KisNodeDummy *dummy);

    void slotIsolatedModeChanged();

    void slotNodeDisplayModeChanged(bool showRootNode, bool showGlobalSelectionMask);

    void processUpdateQueue();
    void progressPercentageChanged(int, const KisNodeSP);

    void slotLayerThumbnailUpdated(KisNodeSP node);

protected:
    virtual KisModelIndexConverterBase *createIndexConverter();
    KisModelIndexConverterBase *indexConverter() const;
    KisDummiesFacadeBase *dummiesFacade() const;

private:
    friend class KisModelIndexConverter;
    friend class KisModelIndexConverterShowAll;

    void connectDummy(KisNodeDummy *dummy, bool needConnect);
    void connectDummies(KisNodeDummy *dummy, bool needConnect);

    void resetIndexConverter();

    void regenerateItems(KisNodeDummy *dummy);
    bool belongsToIsolatedGroup(KisNodeSP node) const;

	void setDropEnabled(const QMimeData *data);
	void updateDropEnabled(const QList<KisNodeSP> &nodes, QModelIndex parent = QModelIndex());

private:

    struct Private;
    Private * const m_d;
};

#endif
