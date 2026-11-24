/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PAINTER_BASED_STROKE_STRATEGY_H
#define __KIS_PAINTER_BASED_STROKE_STRATEGY_H

#include <QVector>

#include "KisRunnableBasedStrokeStrategy.h"
#include "kis_resources_snapshot.h"
#include "kis_selection.h"
#include "kis_indirect_painting_support.h"

class KisPainter;
class KisDistanceInformation;
class KisTransaction;
class KisFreehandStrokeInfo;
class KisMaskedFreehandStrokePainter;
class KisMaskingBrushRenderer;
class KisRunnableStrokeJobData;
class KisUndoStore;

class KRITAUI_EXPORT KisPainterBasedStrokeStrategy : public KisRunnableBasedStrokeStrategy
{
public:
    KisPainterBasedStrokeStrategy(const QLatin1String &id,
                                  const KUndo2MagicString &name,
                                  KisResourcesSnapshotSP resources,
                                  QVector<KisFreehandStrokeInfo*> strokeInfos);

    KisPainterBasedStrokeStrategy(const QLatin1String &id,
                                  const KUndo2MagicString &name,
                                  KisResourcesSnapshotSP resources,
                                  KisFreehandStrokeInfo *strokeInfo);

    ~KisPainterBasedStrokeStrategy();

    void initStrokeCallback() override;
    void finishStrokeCallback() override;
    void cancelStrokeCallback() override;

    void suspendStrokeCallback() override;
    void resumeStrokeCallback() override;

protected:
    KisNodeSP targetNode() const;
    KisPaintDeviceSP targetDevice() const;
    KisSelectionSP activeSelection() const;

    KisMaskedFreehandStrokePainter* maskedPainter(int strokeInfoId);
    int numMaskedPainters() const;

    void setUndoEnabled(bool value);

    bool needsMaskingUpdates() const;

    QVector<KisRunnableStrokeJobData*> doMaskingBrushUpdates(const QVector<QRect> &rects);

protected:

    void setSupportsMaskingBrush(bool value);

    bool supportsMaskingBrush() const;

    void setSupportsIndirectPainting(bool value);
    bool supportsIndirectPainting() const;

    bool supportsContinuedInterstrokeData() const;
    void setSupportsContinuedInterstrokeData(bool value);

    bool supportsTimedMergeId() const;
    void setSupportsTimedMergeId(bool value);

protected:
    KisPainterBasedStrokeStrategy(const KisPainterBasedStrokeStrategy &rhs, int levelOfDetail);

private:
    void init();
    void initPainters(KisPaintDeviceSP targetDevice, KisPaintDeviceSP maskingDevice,
                      KisSelectionSP selection,
                      bool hasIndirectPainting,
                      const QString &indirectPaintingCompositeOp);
    void deletePainters();
    inline int timedID(const QString &id){
        return int(qHash(id));
    }

private:
    KisResourcesSnapshotSP m_resources;
    QVector<KisFreehandStrokeInfo*> m_strokeInfos;
    QVector<KisFreehandStrokeInfo*> m_maskStrokeInfos;
    QVector<KisMaskedFreehandStrokePainter*> m_maskedPainters;

    QScopedPointer<KisTransaction> m_transaction;

    QScopedPointer<KisMaskingBrushRenderer> m_maskingBrushRenderer;

    KisPaintDeviceSP m_targetDevice;
    KisSelectionSP m_activeSelection;

    std::unique_ptr<KUndo2Command> m_autokeyCommand;

    bool m_useMergeID {false};

    bool m_supportsMaskingBrush {false};
    bool m_supportsIndirectPainting {false};
    bool m_supportsContinuedInterstrokeData {false};

    KisIndirectPaintingSupport::FinalMergeSuspenderSP m_finalMergeSuspender;

    struct FakeUndoData {
        FakeUndoData();
        ~FakeUndoData();
        QScopedPointer<KisUndoStore> undoStore;
        QScopedPointer<KisPostExecutionUndoAdapter> undoAdapter;
    };
    QScopedPointer<FakeUndoData> m_fakeUndoData;

};

#endif
