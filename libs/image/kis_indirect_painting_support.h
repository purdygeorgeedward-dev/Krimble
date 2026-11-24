/*
 *  SPDX-FileCopyrightText: 2004 Bart Coppens <kde@bartcoppens.be>
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_INDIRECT_PAINTING_SUPPORT_H_
#define KIS_INDIRECT_PAINTING_SUPPORT_H_

#include "kritaimage_export.h"
#include "kis_types.h"

#include <mutex>

class QBitArray;
class KisUndoAdapter;
class KisPostExecutionUndoAdapter;
class KisPainter;
class KUndo2MagicString;
class KoCompositeOp;
class KoColor;
class KisRunnableStrokeJobData;

class KRITAIMAGE_EXPORT KisIndirectPaintingSupport
{
    KisIndirectPaintingSupport(const KisIndirectPaintingSupport&);
    KisIndirectPaintingSupport& operator=(const KisIndirectPaintingSupport&);
public:

    KisIndirectPaintingSupport();
    virtual ~KisIndirectPaintingSupport();

    bool hasTemporaryTarget() const;

    virtual void setCurrentColor(const KoColor &color);
    void setTemporaryTarget(KisPaintDeviceSP t);
    void setTemporaryCompositeOp(const QString &id);
    void setTemporaryOpacity(qreal o);
    void setTemporaryChannelFlags(const QBitArray& channelFlags);
    void setTemporarySelection(KisSelectionSP selection);

    void setupTemporaryPainter(KisPainter *painter) const;

    void mergeToLayer(KisNodeSP layer, KUndo2Command *parentCommand, const KUndo2MagicString &transactionText, int timedID);
    virtual void mergeToLayerThreaded(KisNodeSP layer, KUndo2Command *parentCommand, const KUndo2MagicString &transactionText, int timedID, QVector<KisRunnableStrokeJobData *> *jobs);

    KisPaintDeviceSP temporaryTarget() const;

    virtual bool supportsNonIndirectPainting() const;

    struct ReadLocker {
        ReadLocker(const KisIndirectPaintingSupport *lock) : m_lock(lock) {
            m_lock->lockTemporaryTarget();
        }
        ~ReadLocker() {
            m_lock->unlockTemporaryTarget();
        }

    private:
        const KisIndirectPaintingSupport *m_lock;
    };

    struct FinalMergeSuspender {
        FinalMergeSuspender(KisIndirectPaintingSupport *indirect);
        ~FinalMergeSuspender();

    private:
        const KisIndirectPaintingSupport *m_lock;
    };
    using FinalMergeSuspenderSP = QSharedPointer<FinalMergeSuspender>;

    FinalMergeSuspenderSP trySuspendFinalMerge();

protected:
    struct WriteLocker {
        WriteLocker(KisIndirectPaintingSupport *lock)
            : m_lock(lock),
              m_locked(true)
        {
            m_lock->lockTemporaryTargetForWrite();
        }

        WriteLocker(KisIndirectPaintingSupport *lock, std::defer_lock_t)
            : m_lock(lock),
              m_locked(false)
        {
        }

        ~WriteLocker() {
            if (m_locked) {
                m_lock->unlockTemporaryTarget();
            }
        }

        void unlock() {
            KIS_SAFE_ASSERT_RECOVER_RETURN(m_locked);
            m_lock->unlockTemporaryTarget();
            m_locked = false;
        }

        void relock() {
            KIS_SAFE_ASSERT_RECOVER_RETURN(!m_locked);
            m_lock->lockTemporaryTargetForWrite();
            m_locked = true;
        }

        bool isLocked() {
            return m_locked;
        }

    private:
        KisIndirectPaintingSupport *m_lock;
        bool m_locked = false;
    };

    using WriteLockerSP = QSharedPointer<WriteLocker>;

    void mergeToLayerImpl(KisPaintDeviceSP dst, KUndo2Command *parentCommand, const KUndo2MagicString &transactionText, int timedID, bool cleanResources, WriteLockerSP sharedWriteLock, QVector<KisRunnableStrokeJobData *> *jobs);
    virtual void writeMergeData(KisPainter *painter, KisPaintDeviceSP src, const QRect &rc);
    void lockTemporaryTargetForWrite() const;

    QString temporaryCompositeOp() const;
    void releaseResources();

private:
    void lockTemporaryTarget() const;

    void unlockTemporaryTarget() const;

private:
    friend class KisPainterBasedStrokeStrategy;

    KisSelectionSP temporarySelection() const;

private:
    struct Private;
    Private* const d;
};

#endif
