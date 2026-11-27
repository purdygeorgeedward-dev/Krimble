/*
 *  SPDX-FileCopyrightText: 2020 Saurabh Kumar <saurabhk660@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef STORYBOARD_MODEL
#define STORYBOARD_MODEL

#include "StoryboardItem.h"
#include "CommentModel.h"

#include <QAbstractListModel>
#include <QItemSelection>

#include <kis_keyframe_channel.h>
#include "kis_idle_watcher.h"
#include <kritastoryboarddocker_export.h>
#include <kis_image.h>
#include <kis_signal_compressor.h>

#include <kis_node.h>
#include <kis_paint_device.h>
class StoryboardView;
class KisTimeSpan;
class KisStoryboardThumbnailRenderScheduler;
class KUndo2Command;

class KRITASTORYBOARDDOCKER_EXPORT StoryboardModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum AdditionalRoles {
        TotalSceneDurationInFrames = Qt::UserRole + 1,
        TotalSceneDurationInSeconds = Qt::UserRole + 2,
    };

    class KeyframeReorderLock {
    public:
        KeyframeReorderLock(StoryboardModel* model)
            : m_model(model)
            , m_originalLock(!model->m_reorderingKeyframes) {
            m_model->m_reorderingKeyframes = true;
        }

        ~KeyframeReorderLock() {
            m_model->m_reorderingKeyframes = !m_originalLock;
        }

    private:
        StoryboardModel* m_model;
        bool m_originalLock = false;
    };

    StoryboardModel(QObject *parent);
    ~StoryboardModel() override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setCommentScrollData(const QModelIndex & index, const QVariant & value);

    bool setThumbnailPixmapData(const QModelIndex & parentIndex, const KisPaintDeviceSP & dev);

    bool updateDurationData(const QModelIndex & parentIndex);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex())override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;

    int visibleCommentCount() const;

    int totalCommentCount();

    int visibleCommentsUpto(QModelIndex index) const;

    void setCommentModel(StoryboardCommentModel *commentModel);

    StoryboardComment getComment(int row) const;

    void setFreeze(bool);
    bool isFrozen() const;
    void setLocked(bool);
    bool isLocked() const;
    int getFramesPerSecond() const;
    void setView(StoryboardView *view);
    void setImage(KisImageWSP image);

    QModelIndex indexFromFrame(int frame, bool framePerfect = true) const;

    QModelIndex lastIndexBeforeFrame(int frame) const;

    QModelIndexList affectedIndexes(KisTimeSpan range) const;

    int nextKeyframeGlobal(int keyframeTime) const;

    int lastKeyframeWithin(QModelIndex index);

    void reorderKeyframes();

    bool changeSceneHoldLength(int oldDuration, QModelIndex itemIndex);

    bool insertItem(QModelIndex index, bool after);

    bool removeItem(QModelIndex index, KUndo2Command *command = nullptr);

    void resetData(StoryboardItemList list);

    StoryboardItemList getData();

    void pushUndoCommand(KUndo2Command *command);

    void shiftKeyframes(KisTimeSpan affected, int offset, KUndo2Command *cmd = nullptr);

    int lastKeyframeGlobal() const;
    void slotUpdateThumbnailsForItems(QModelIndexList indices);

    void insertChildRows(int position, KUndo2Command* cmd = nullptr);

    void insertChildRows(int position, StoryboardItemSP item);

    void visualizeScene(const QModelIndex& index, bool useUndo = true);

    void createDuplicateKeyframes(const QModelIndex& index, KUndo2Command* cmd = nullptr);
    void createBlankKeyframes(const QModelIndex& index, KUndo2Command* cmd = nullptr);

private:
    inline bool isValidBoard(const QModelIndex &index) const {return index.isValid() && !index.parent().isValid();}

    bool moveRowsImpl(const QModelIndex &sourceParent, int sourceRow, int count,
                    const QModelIndex &destinationParent, int destinationChild, KUndo2Command *parentCMD = nullptr);

private Q_SLOTS:
    void slotCurrentFrameChanged(int frameId);
    void slotKeyframeAdded(const KisKeyframeChannel *channel, int time);
    void slotKeyframeRemoved(const KisKeyframeChannel *channel, int time);
    void slotNodeRemoved(KisNodeSP node);

    void slotFramerateChanged();

    void slotUpdateThumbnailForFrame(int frame, bool delay = true);

    void slotUpdateThumbnails();

    void slotFrameRenderCompleted(int frame, KisPaintDeviceSP dev);

    void slotFrameRenderCancelled(int frame);

    void slotCommentDataChanged();
    void slotCommentRowInserted(const QModelIndex, int, int);
    void slotCommentRowRemoved(const QModelIndex, int, int);
    void slotCommentRowMoved(const QModelIndex &sourceParent, int sourceRow, int count,
                            const QModelIndex &destinationParent, int destinationChild);

public Q_SLOTS:
    void slotSetActiveNode(KisNodeSP);

Q_SIGNALS:
    void sigStoryboardItemListChanged();

private:
    friend class KisMoveStoryboardCommand;

    StoryboardItemList m_items;
    QVector<StoryboardComment> m_commentList;
    StoryboardCommentModel *m_commentModel {0};
    bool m_freezeKeyframePositions {false};
    bool m_lockBoards {false};
    bool m_reorderingKeyframes {false};
    int m_lastScene {0};
    KisIdleWatcher m_imageIdleWatcher;
    KisImageWSP m_image;
    StoryboardView *m_view {0};
    KisNodeWSP m_activeNode;
    KisStoryboardThumbnailRenderScheduler *m_renderScheduler {0};
    KisSignalCompressor m_renderSchedulingCompressor;
};

#endif
