/*------------------------------------------------------------------------
  Junction: Concurrent data structures in C++
  Copyright (c) 2016 Jeff Preshing
  Distributed under the Simplified BSD License.
  Original location: https://github.com/preshing/junction
  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the LICENSE file for more information.
------------------------------------------------------------------------*/

#ifndef LEAPFROG_H
#define LEAPFROG_H

#include "map_traits.h"
#include "simple_job_coordinator.h"
#include "kis_assert.h"

#define SANITY_CHECK

template <class Map>
struct Leapfrog {
    typedef typename Map::Hash Hash;
    typedef typename Map::Value Value;
    typedef typename Map::KeyTraits KeyTraits;
    typedef typename Map::ValueTraits ValueTraits;

    static const quint64 InitialSize = 8;
    static const quint64 TableMigrationUnitSize = 32;
    static const quint64 LinearSearchLimit = 128;
    static const quint64 CellsInUseSample = LinearSearchLimit;

    Q_STATIC_ASSERT(LinearSearchLimit > 0 && LinearSearchLimit < 256);
    Q_STATIC_ASSERT(CellsInUseSample > 0 && CellsInUseSample <= LinearSearchLimit);

    struct Cell {
        Atomic<Hash> hash;
        Atomic<Value> value;
    };

    struct CellGroup {
        Atomic<quint8> deltas[8];
        Cell cells[4];
    };

    struct Table {
        const quint64 sizeMask;
        QMutex mutex;
        SimpleJobCoordinator jobCoordinator;

        Table(quint64 sizeMask) : sizeMask(sizeMask)
        {
        }

        static Table* create(quint64 tableSize)
        {
#ifdef SANITY_CHECK
            KIS_ASSERT_RECOVER_NOOP(isPowerOf2(tableSize));
            KIS_ASSERT_RECOVER_NOOP(tableSize >= 4);
#endif
            quint64 numGroups = tableSize >> 2;
            Table* table = (Table*) std::malloc(sizeof(Table) + sizeof(CellGroup) * numGroups);
            new (table) Table(tableSize - 1);

            for (quint64 i = 0; i < numGroups; i++) {
                CellGroup* group = table->getCellGroups() + i;

                for (quint64 j = 0; j < 4; j++) {
                    group->deltas[j].storeNonatomic(0);
                    group->deltas[j + 4].storeNonatomic(0);
                    group->cells[j].hash.storeNonatomic(KeyTraits::NullHash);
                    group->cells[j].value.storeNonatomic(Value(ValueTraits::NullValue));
                }
            }
            return table;
        }

        void destroy()
        {
            this->Table::~Table();
            std::free(this);
        }

        CellGroup* getCellGroups() const
        {
            return (CellGroup*)(this + 1);
        }

        quint64 getNumMigrationUnits() const
        {
            return sizeMask / TableMigrationUnitSize + 1;
        }
    };

    class TableMigration : public SimpleJobCoordinator::Job
    {
    public:
        struct Source {
            Table* table;
            Atomic<quint64> sourceIndex;
        };

        Map& m_map;
        Table* m_destination {nullptr};
        Atomic<quint64> m_workerStatus;
        Atomic<bool> m_overflowed;
        Atomic<qint64> m_unitsRemaining;
        quint64 m_numSources {0};

        TableMigration(Map& map) : m_map(map)
        {
        }

        static TableMigration* create(Map& map, quint64 numSources)
        {
            TableMigration* migration =
                (TableMigration*) std::malloc(sizeof(TableMigration) + sizeof(TableMigration::Source) * numSources);
            new (migration) TableMigration(map);

            migration->m_workerStatus.storeNonatomic(0);
            migration->m_overflowed.storeNonatomic(false);
            migration->m_unitsRemaining.storeNonatomic(0);
            migration->m_numSources = numSources;
            return migration;
        }

        virtual ~TableMigration() override
        {
        }

        void destroy()
        {
            for (quint64 i = 0; i < m_numSources; i++)
                if (getSources()[i].table)
                    getSources()[i].table->destroy();
            this->TableMigration::~TableMigration();
            std::free(this);
        }

        Source* getSources() const
        {
            return (Source*)(this + 1);
        }

        bool migrateRange(Table* srcTable, quint64 startIdx);
        virtual void run() override;
    };

    static Cell* find(Hash hash, Table* table)
    {
#ifdef SANITY_CHECK
        KIS_ASSERT_RECOVER_NOOP(table);
        KIS_ASSERT_RECOVER_NOOP(hash != KeyTraits::NullHash);
#endif
        quint64 sizeMask = table->sizeMask;
        quint64 idx = hash & sizeMask;
        CellGroup* group = table->getCellGroups() + (idx >> 2);
        Cell* cell = group->cells + (idx & 3);
        Hash probeHash = cell->hash.load(Relaxed);

        if (probeHash == hash) {
            return cell;
        } else if (probeHash == KeyTraits::NullHash) {
            return cell = NULL;
        }
        quint8 delta = group->deltas[idx & 3].load(Relaxed);
        while (delta) {
            idx = (idx + delta) & sizeMask;
            group = table->getCellGroups() + (idx >> 2);
            cell = group->cells + (idx & 3);
            Hash probeHash = cell->hash.load(Relaxed);
            if (probeHash == hash) {
                return cell;
            }
            delta = group->deltas[(idx & 3) + 4].load(Relaxed);
        }
        return NULL;
    }

    enum InsertResult { InsertResult_AlreadyFound, InsertResult_InsertedNew, InsertResult_Overflow };
    static InsertResult insertOrFind(Hash hash, Table* table, Cell*& cell, quint64& overflowIdx)
    {
#ifdef SANITY_CHECK
        KIS_ASSERT_RECOVER_NOOP(table);
        KIS_ASSERT_RECOVER_NOOP(hash != KeyTraits::NullHash);
#endif
        quint64 sizeMask = table->sizeMask;
        quint64 idx = quint64(hash);

        CellGroup* group = table->getCellGroups() + ((idx & sizeMask) >> 2);
        cell = group->cells + (idx & 3);
        Hash probeHash = cell->hash.load(Relaxed);

        if (probeHash == KeyTraits::NullHash) {
            if (cell->hash.compareExchangeStrong(probeHash, hash, Relaxed)) {
                return InsertResult_InsertedNew;
            } else {
            }
        }

        if (probeHash == hash) {
            return InsertResult_AlreadyFound;
        }

        quint64 maxIdx = idx + sizeMask;
        quint64 linkLevel = 0;
        Atomic<quint8>* prevLink;
        for (;;) {
        followLink:
            prevLink = group->deltas + ((idx & 3) + linkLevel);
            linkLevel = 4;
            quint8 probeDelta = prevLink->load(Relaxed);

            if (probeDelta) {
                idx += probeDelta;
                group = table->getCellGroups() + ((idx & sizeMask) >> 2);
                cell = group->cells + (idx & 3);
                probeHash = cell->hash.load(Relaxed);

                if (probeHash == KeyTraits::NullHash) {
                    do {
                        probeHash = cell->hash.load(Acquire);
                    } while (probeHash == KeyTraits::NullHash);
                }

#ifdef SANITY_CHECK
                KIS_ASSERT_RECOVER_NOOP(((probeHash ^ hash) & sizeMask) == 0);
#endif
                if (probeHash == hash) {
                    return InsertResult_AlreadyFound;
                }
            } else {
                quint64 prevLinkIdx = idx;
#ifdef SANITY_CHECK
                KIS_ASSERT_RECOVER_NOOP(qint64(maxIdx - idx) >= 0);
#endif
                quint64 linearProbesRemaining = qMin(maxIdx - idx, quint64(LinearSearchLimit));

                while (linearProbesRemaining-- > 0) {
                    idx++;
                    group = table->getCellGroups() + ((idx & sizeMask) >> 2);
                    cell = group->cells + (idx & 3);
                    probeHash = cell->hash.load(Relaxed);

                    if (probeHash == KeyTraits::NullHash) {
                        if (cell->hash.compareExchangeStrong(probeHash, hash, Relaxed)) {
#ifdef SANITY_CHECK
                            KIS_ASSERT_RECOVER_NOOP(probeDelta == 0);
#endif
                            quint8 desiredDelta = idx - prevLinkIdx;
                            prevLink->store(desiredDelta, Relaxed);
                            return InsertResult_InsertedNew;
                        } else {
                        }
                    }
                    Hash x = (probeHash ^ hash);
                    if (!x) {
                        return InsertResult_AlreadyFound;
                    }
                    if ((x & sizeMask) == 0) {
                        quint8 desiredDelta = idx - prevLinkIdx;
                        prevLink->store(desiredDelta, Relaxed);
                        goto followLink;
                    }
                }
                overflowIdx = idx + 1;
                return InsertResult_Overflow;
            }
        }
    }

    static void beginTableMigrationToSize(Map& map, Table* table, quint64 nextTableSize)
    {
        SimpleJobCoordinator::Job* job = table->jobCoordinator.loadConsume();
        if (job) {
        } else {
            QMutexLocker guard(&table->mutex);
            job = table->jobCoordinator.loadConsume();

            if (job) {
            } else {
                TableMigration* migration = TableMigration::create(map, 1);
                migration->m_unitsRemaining.storeNonatomic(table->getNumMigrationUnits());
                migration->getSources()[0].table = table;
                migration->getSources()[0].sourceIndex.storeNonatomic(0);
                migration->m_destination = Table::create(nextTableSize);
                table->jobCoordinator.storeRelease(migration);
            }
        }
    }

    static void beginTableMigration(Map& map, Table* table, quint64 overflowIdx)
    {
        quint64 sizeMask = table->sizeMask;
        quint64 idx = overflowIdx - CellsInUseSample;
        quint64 inUseCells = 0;
        for (quint64 linearProbesRemaining = CellsInUseSample; linearProbesRemaining > 0; linearProbesRemaining--) {
            CellGroup* group = table->getCellGroups() + ((idx & sizeMask) >> 2);
            Cell* cell = group->cells + (idx & 3);
            Value value = cell->value.load(Relaxed);
            if (value == Value(ValueTraits::Redirect)) {
                return;
            }
            if (value != Value(ValueTraits::NullValue))
                inUseCells++;
            idx++;
        }
        float inUseRatio = float(inUseCells) / CellsInUseSample;
        float estimatedInUse = (sizeMask + 1) * inUseRatio;
        quint64 nextTableSize = qMax(quint64(InitialSize), roundUpPowerOf2(quint64(estimatedInUse * 2)));
        beginTableMigrationToSize(map, table, nextTableSize);
    }
};

template <class Map>
bool Leapfrog<Map>::TableMigration::migrateRange(Table* srcTable, quint64 startIdx)
{
    quint64 srcSizeMask = srcTable->sizeMask;
    quint64 endIdx = qMin(startIdx + TableMigrationUnitSize, srcSizeMask + 1);
    for (quint64 srcIdx = startIdx; srcIdx < endIdx; srcIdx++) {
        CellGroup* srcGroup = srcTable->getCellGroups() + ((srcIdx & srcSizeMask) >> 2);
        Cell* srcCell = srcGroup->cells + (srcIdx & 3);
        Hash srcHash;
        Value srcValue;
        for (;;) {
            srcHash = srcCell->hash.load(Relaxed);
            if (srcHash == KeyTraits::NullHash) {
                srcValue =
                    srcCell->value.compareExchange(Value(ValueTraits::NullValue), Value(ValueTraits::Redirect), Relaxed);
                if (srcValue == Value(ValueTraits::Redirect)) {
                    break;
                }
                if (srcValue == Value(ValueTraits::NullValue)) {
                    break;
                }
            } else {
                srcValue = srcCell->value.load(Relaxed);
                if (srcValue == Value(ValueTraits::NullValue)) {
                    if (srcCell->value.compareExchangeStrong(srcValue, Value(ValueTraits::Redirect), Relaxed)) {
                        break;
                    }

                    if (srcValue == Value(ValueTraits::Redirect)) {
                        break;
                    }
                } else if (srcValue == Value(ValueTraits::Redirect)) {
                    break;
                }

#ifdef SANITY_CHECK
                KIS_ASSERT_RECOVER_NOOP(srcHash != KeyTraits::NullHash);
                KIS_ASSERT_RECOVER_NOOP(srcValue != Value(ValueTraits::NullValue));
                KIS_ASSERT_RECOVER_NOOP(srcValue != Value(ValueTraits::Redirect));
#endif
                Cell* dstCell;
                quint64 overflowIdx;
                InsertResult result = insertOrFind(srcHash, m_destination, dstCell, overflowIdx);
#ifdef SANITY_CHECK
                KIS_ASSERT_RECOVER_NOOP(result != InsertResult_AlreadyFound);
#endif
                if (result == InsertResult_Overflow) {
                    return false;
                }
                for (;;) {
                    dstCell->value.store(srcValue, Relaxed);
                    Value doubleCheckedSrcValue = srcCell->value.compareExchange(srcValue, Value(ValueTraits::Redirect), Relaxed);
#ifdef SANITY_CHECK
                    KIS_ASSERT_RECOVER_NOOP(doubleCheckedSrcValue != Value(ValueTraits::Redirect));
#endif
                    if (doubleCheckedSrcValue == srcValue) {
                        if (srcValue == Value(ValueTraits::NullValue)) {
                        }

                        break;
                    }
                    srcValue = doubleCheckedSrcValue;
                }
                break;
            }
        }
    }
    return true;
}

template <class Map>
void Leapfrog<Map>::TableMigration::run()
{
#ifdef SANITY_CHECK
    KIS_ASSERT_RECOVER_NOOP(m_map.getGC().sanityRawPointerAccessLocked());
#endif

    quint64 probeStatus = m_workerStatus.load(Relaxed);
    do {
        if (probeStatus & 1) {
            return;
        }
    } while (!m_workerStatus.compareExchangeWeak(probeStatus, probeStatus + 2, Relaxed, Relaxed));
#ifdef SANITY_CHECK
    KIS_ASSERT_RECOVER_NOOP((probeStatus & 1) == 0);
#endif

    for (quint64 s = 0; s < m_numSources; s++) {
        Source& source = getSources()[s];
        for (;;) {
            if (m_workerStatus.load(Relaxed) & 1) {
                goto endMigration;
            }
            quint64 startIdx = source.sourceIndex.fetchAdd(TableMigrationUnitSize, Relaxed);
            if (startIdx >= source.table->sizeMask + 1)
                break;
            bool overflowed = !migrateRange(source.table, startIdx);
            if (overflowed) {
                bool oldOverflowed = m_overflowed.exchange(overflowed, Relaxed);
                if (oldOverflowed) {
                }

                m_workerStatus.fetchOr(1, Relaxed);
                goto endMigration;
            }

            qint64 prevRemaining = m_unitsRemaining.fetchSub(1, Relaxed);
#ifdef SANITY_CHECK
            KIS_ASSERT_RECOVER_NOOP(prevRemaining > 0);
#endif
            if (prevRemaining == 1) {
                m_workerStatus.fetchOr(1, Relaxed);
                goto endMigration;
            }
        }
    }

endMigration:
    probeStatus = m_workerStatus.fetchSub(2, AcquireRelease);
    if (probeStatus >= 4) {
        return;
    }

#ifdef SANITY_CHECK
    KIS_ASSERT_RECOVER_NOOP(probeStatus == 3);
#endif
    bool overflowed = m_overflowed.loadNonatomic();
    if (!overflowed) {
        m_map.publishTableMigration(this);
        getSources()[0].table->jobCoordinator.end();
    } else {
        Table* origTable = getSources()[0].table;
        QMutexLocker guard(&origTable->mutex);
        SimpleJobCoordinator::Job* checkedJob = origTable->jobCoordinator.loadConsume();

        if (checkedJob != this) {
        } else {
            TableMigration* migration = TableMigration::create(m_map, m_numSources + 1);
            migration->m_destination = Table::create((m_destination->sizeMask + 1) * 2);
            for (quint64 i = 0; i < m_numSources; i++) {
                migration->getSources()[i].table = getSources()[i].table;
                getSources()[i].table = NULL;
                migration->getSources()[i].sourceIndex.storeNonatomic(0);
            }

            migration->getSources()[m_numSources].table = m_destination;
            migration->getSources()[m_numSources].sourceIndex.storeNonatomic(0);
            quint64 unitsRemaining = 0;
            for (quint64 s = 0; s < migration->m_numSources; s++) {
                unitsRemaining += migration->getSources()[s].table->getNumMigrationUnits();
            }

            migration->m_unitsRemaining.storeNonatomic(unitsRemaining);
            origTable->jobCoordinator.storeRelease(migration);
        }
    }

    m_map.getGC().enqueue(&TableMigration::destroy, this, true);
}

#endif
