/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISLAZYSHAREDCACHESTORAGE_H
#define KISLAZYSHAREDCACHESTORAGE_H

#include <type_traits>
#include <functional>
#include <utility>

#include <QMutex>
#include <QAtomicPointer>
#include <QSharedPointer>
#include <QSharedDataPointer>
#include <QExplicitlySharedDataPointer>

namespace KisLazySharedCacheStorageDetail
{

template <typename StorageType, typename T, typename... Args>
struct DataStorage
{
    using ConstType = std::add_const_t<T>;
    using FactoryType = T*(Args...);

    DataStorage() {}
    DataStorage(T *value) : m_value(value) {}
    DataStorage(const DataStorage &rhs) = default;
    DataStorage& operator=(const DataStorage &rhs) = default;

    ConstType* lazyInitialize(const std::function<FactoryType> &factory, Args... args) {
        if (!m_value) {
            m_value.reset(factory(std::forward<Args...>(args...)));
        }
        return m_value.data();
    }

    bool hasValue() const {
        return !m_value.isNull();
    }

    void reset() {
        m_value.reset();
    }

private:
    StorageType m_value;
};

template <typename T, typename... Args>
using DataWrapperLocal = DataStorage<QSharedPointer<T>, T, Args...>;

template <typename T, typename... Args>
struct DataWrapperShared
{
    using ConstType = std::add_const_t<T>;
    using FactoryType = T*(Args...);

private:
    struct SharedStorage
    {
        using ConstType = std::add_const_t<T>;

        SharedStorage() {}
        SharedStorage(T *value) : m_value(value) {}
        SharedStorage(const SharedStorage &rhs) = delete;

        QMutex sharedMutex;
        DataStorage<QScopedPointer<ConstType>, T, Args...> m_value;
    };
public:

    DataWrapperShared() : m_sharedStorage(new SharedStorage()) {}
    DataWrapperShared(T *value) : m_sharedStorage(new SharedStorage(value)) {}
    DataWrapperShared(const DataWrapperShared &rhs) = default;
    DataWrapperShared& operator=(const DataWrapperShared &rhs) = default;

    ConstType* lazyInitialize(const std::function<FactoryType> &factory, Args... args) {
        QMutexLocker l(&m_sharedStorage->sharedMutex);
        return m_sharedStorage->m_value.lazyInitialize(factory, std::forward<Args...>(args...));
    }

    bool hasValue() const {
        QMutexLocker l(&m_sharedStorage->sharedMutex);
        return m_sharedStorage->m_value.hasValue();
    }

    void reset() {
        m_sharedStorage.reset(new SharedStorage());
    }

private:
    QSharedPointer<SharedStorage> m_sharedStorage;
};

}

template <typename DataWrapper, typename T, typename... Args>
class KisLazySharedCacheStorageBase
{
public:
    using ConstType = std::add_const_t<T>;
    using FactoryType = T*(Args...);

public:
    KisLazySharedCacheStorageBase()
    {
    }

    KisLazySharedCacheStorageBase(std::function<FactoryType> factory)
        : m_factory(factory)
    {
    }

    KisLazySharedCacheStorageBase(const KisLazySharedCacheStorageBase &rhs) {
        QMutexLocker l(&rhs.m_mutex);
        m_factory = rhs.m_factory;
        m_dataWrapper = rhs.m_dataWrapper;
        m_cachedValue = rhs.m_cachedValue;
    }

    void setFactory(std::function<FactoryType> factory) {
        m_factory = factory;
    }

    ConstType* value(Args... args) {
        ConstType *result = 0;

        if (m_cachedValue) {
            result = m_cachedValue;
        } else {
            QMutexLocker l1(&m_mutex);
            m_cachedValue = m_dataWrapper.lazyInitialize(m_factory, args...);
            result = m_cachedValue;
        }
        return result;
    }

    bool isNull() const {
        return !bool(m_cachedValue) && !m_dataWrapper.hasValue();
    }

    void initialize(Args... args) {
        (void) value(args...);
    }

    void reset() {
        QMutexLocker l(&m_mutex);
        m_cachedValue.storeRelaxed(nullptr);
        m_dataWrapper.reset();
    }

private:
    std::function<FactoryType> m_factory;
    DataWrapper m_dataWrapper;
    QAtomicPointer<ConstType> m_cachedValue;
    mutable QMutex m_mutex;
};

template <typename T, typename... Args>
using KisLazySharedCacheStorage =
    KisLazySharedCacheStorageBase<
        KisLazySharedCacheStorageDetail::DataWrapperLocal<T, Args...>, T, Args...>;

template <typename T, typename... Args>
using KisLazySharedCacheStorageLinked =
    KisLazySharedCacheStorageBase<
        KisLazySharedCacheStorageDetail::DataWrapperShared<T, Args...>, T, Args...>;

#endif
