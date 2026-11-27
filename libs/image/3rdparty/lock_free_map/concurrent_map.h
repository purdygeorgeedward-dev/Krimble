/*------------------------------------------------------------------------
  Junction: Concurrent data structures in C++
  Copyright (c) 2016 Jeff Preshing
  Distributed under the Simplified BSD License.
  Original location: https://github.com/preshing/junction
  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the LICENSE file for more information.
------------------------------------------------------------------------*/

#ifndef CONCURRENTMAP_H
#define CONCURRENTMAP_H

#include "leapfrog.h"
#include "qsbr.h"

template <typename K, typename V, class KT = DefaultKeyTraits<K>, class VT = DefaultValueTraits<V> >
class ConcurrentMap
{
public:
    typedef K Key;
    typedef V Value;
    typedef KT KeyTraits;
    typedef VT ValueTraits;
    typedef quint32 Hash;
    typedef Leapfrog<ConcurrentMap> Details;

private:
    Atomic<typename Details::Table*> m_root;
    QSBR m_gc;

public:
    ConcurrentMap(quint64 capacity = Details::InitialSize) : m_root(Details::Table::create(capacity))
    {
    }

    ~ConcurrentMap()
    {
        typename Details::Table* table = m_root.loadNonatomic();
        table->destroy();
        m_gc.flush();
    }

    QSBR &getGC()
    {
        return m_gc;
    }

    bool migrationInProcess()
    {
        return quint64(m_root.loadNonatomic()->jobCoordinator.loadConsume()) > 1;
    }

    void publishTableMigration(typename Details::TableMigration* migration)
    {
        m_root.store(migration->m_destination, Release);
    }

    class Mutator
    {
    private:
        friend class ConcurrentMap;

        ConcurrentMap& m_map;
        typename Details::Table* m_table;
        typename Details::Cell* m_cell;
        Value m_value;

        Mutator(ConcurrentMap& map, Key key, bool) : m_map(map), m_value(Value(ValueTraits::NullValue))
        {
            Hash hash = KeyTraits::hash(key);
            for (;;) {
                m_table = m_map.m_root.load(Consume);
                m_cell = Details::find(hash, m_table);
                if (!m_cell) {
                    return;
                }

                Value value = m_cell->value.load(Consume);
                if (value != Value(ValueTraits::Redirect)) {
                    m_value = value;
                    return;
                }
                m_table->jobCoordinator.participate();
            }
        }

        Mutator(ConcurrentMap& map, Key key) : m_map(map), m_value(Value(ValueTraits::NullValue))
        {
            Hash hash = KeyTraits::hash(key);
            for (;;) {
                m_table = m_map.m_root.load(Consume);
                quint64 overflowIdx;
                switch (Details::insertOrFind(hash, m_table, m_cell, overflowIdx)) {
                case Details::InsertResult_InsertedNew: {
                    return;
                }
                case Details::InsertResult_AlreadyFound: {
                    Value value = m_cell->value.load(Consume);
                    if (value == Value(ValueTraits::Redirect)) {
                        break;
                    }
                    m_value = value;
                    return;
                }
                case Details::InsertResult_Overflow: {
                    Details::beginTableMigration(m_map, m_table, overflowIdx);
                    break;
                }
                }
                m_table->jobCoordinator.participate();
            }
        }

    public:
        Value getValue() const
        {
            return Value(m_value);
        }

        Value exchangeValue(Value desired)
        {
            for (;;) {
                Value oldValue = m_value;
                if (m_cell->value.compareExchangeStrong(m_value, desired, ConsumeRelease)) {
                    Value result = m_value;
                    m_value = desired;
                    return result;
                }
                if (m_value != Value(ValueTraits::Redirect)) {
                    if (oldValue == Value(ValueTraits::NullValue) && m_value != Value(ValueTraits::NullValue)) {
                    }
                    return desired;
                }

                Hash hash = m_cell->hash.load(Relaxed);
                for (;;) {
                    m_table->jobCoordinator.participate();
                    m_table = m_map.m_root.load(Consume);
                    m_value = Value(ValueTraits::NullValue);
                    quint64 overflowIdx;

                    switch (Details::insertOrFind(hash, m_table, m_cell, overflowIdx)) {
                    case Details::InsertResult_AlreadyFound:
                        m_value = m_cell->value.load(Consume);
                        if (m_value == Value(ValueTraits::Redirect)) {
                            break;
                        }
                        goto breakOuter;
                    case Details::InsertResult_InsertedNew:
                        goto breakOuter;
                    case Details::InsertResult_Overflow:
                        Details::beginTableMigration(m_map, m_table, overflowIdx);
                        break;
                    }
                }
            breakOuter:;
            }
        }

        void assignValue(Value desired)
        {
            exchangeValue(desired);
        }

        Value eraseValue()
        {
            for (;;) {
                if (m_value == Value(ValueTraits::NullValue)) {
                    return Value(m_value);
                }

                if (m_cell->value.compareExchangeStrong(m_value, Value(ValueTraits::NullValue), Consume)) {
                    Value result = m_value;
                    m_value = Value(ValueTraits::NullValue);
                    return result;
                }

                if (m_value != Value(ValueTraits::Redirect)) {
                    return Value(ValueTraits::NullValue);
                }

                Hash hash = m_cell->hash.load(Relaxed);
                for (;;) {
                    m_table->jobCoordinator.participate();
                    m_table = m_map.m_root.load(Consume);
                    m_cell = Details::find(hash, m_table);
                    if (!m_cell) {
                        m_value = Value(ValueTraits::NullValue);
                        return m_value;
                    }

                    m_value = m_cell->value.load(Relaxed);
                    if (m_value != Value(ValueTraits::Redirect)) {
                        break;
                    }
                }
            }
        }
    };

    Mutator insertOrFind(Key key)
    {
        return Mutator(*this, key);
    }

    Mutator find(Key key)
    {
        return Mutator(*this, key, false);
    }

    Value get(Key key)
    {
        Hash hash = KeyTraits::hash(key);
        for (;;) {
            typename Details::Table* table = m_root.load(Consume);
            typename Details::Cell* cell = Details::find(hash, table);
            if (!cell) {
                return Value(ValueTraits::NullValue);
            }

            Value value = cell->value.load(Consume);
            if (value != Value(ValueTraits::Redirect)) {
                return value;
            }
            table->jobCoordinator.participate();
        }
    }

    Value assign(Key key, Value desired)
    {
        Mutator iter(*this, key);
        return iter.exchangeValue(desired);
    }

    Value exchange(Key key, Value desired)
    {
        Mutator iter(*this, key);
        return iter.exchangeValue(desired);
    }

    Value erase(Key key)
    {
        Mutator iter(*this, key, false);
        return iter.eraseValue();
    }

    class Iterator
    {
    private:
        typename Details::Table* m_table;
        quint64 m_idx;
        Key m_hash;
        Value m_value;

    public:
        Iterator() = default;
        Iterator(ConcurrentMap& map)
        {
            m_table = map.m_root.load(Consume);
            m_idx = -1;
            next();
        }

        void setMap(ConcurrentMap& map)
        {
            m_table = map.m_root.load(Consume);
            m_idx = -1;
            next();
        }

        void next()
        {
            while (++m_idx <= m_table->sizeMask) {
                typename Details::CellGroup* group = m_table->getCellGroups() + (m_idx >> 2);
                typename Details::Cell* cell = group->cells + (m_idx & 3);
                m_hash = cell->hash.load(Relaxed);

                if (m_hash != KeyTraits::NullHash) {
                    m_value = cell->value.load(Relaxed);
                    if (m_value != Value(ValueTraits::NullValue))
                        return;
                }
            }
            m_hash = KeyTraits::NullHash;
            m_value = Value(ValueTraits::NullValue);
        }

        bool isValid() const
        {
#ifdef SANITY_CHECK
            KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(m_value != Value(ValueTraits::Redirect), false);
#endif
            return m_value != Value(ValueTraits::NullValue);
        }

        Key getKey() const
        {
            return KeyTraits::dehash(m_hash);
        }

        Value getValue() const
        {
            return m_value;
        }
    };
};

#endif
