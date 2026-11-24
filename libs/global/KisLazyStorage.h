/*
 *  SPDX-FileCopyrightText: 2018 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISLAZYSTORAGE_H
#define KISLAZYSTORAGE_H

#include <functional>
#include <memory>
#include <atomic>
#include <mutex>

#include "KisMpl.h"

template <typename T, typename... Args>
class KisLazyStorage
{
public:
    struct init_value_tag { explicit init_value_tag() = default; };

    explicit KisLazyStorage(Args... args)
        : m_constructionArgs(std::forward<Args>(args)...),
          m_data(0)
    {
    }

    explicit KisLazyStorage(init_value_tag, T &&value)
        : m_data(new T(std::forward<T>(value)))
    {
    }

    KisLazyStorage(const KisLazyStorage &rhs) = delete;
    KisLazyStorage& operator=(const KisLazyStorage &rhs) = delete;

    KisLazyStorage(KisLazyStorage &&rhs) {
        *this = std::move(rhs);
    }

    KisLazyStorage& operator=(KisLazyStorage &&rhs) {
        std::scoped_lock lock(m_mutex, rhs.m_mutex);

        m_constructionArgs = std::move(rhs.m_constructionArgs);
        delete m_data.load();
        m_data.store(rhs.m_data.load());
        rhs.m_data.store(0);

        return *this;
    }

    ~KisLazyStorage() {
        delete m_data.load();
    }

    T* operator->() {
        return getPointer();
    }

    T& operator*() {
        return *getPointer();
    }

private:
    T* getPointer() {
        if(!m_data) {
            std::unique_lock l(m_mutex);
            if(!m_data) {
                m_data = std::apply(&constructObject, m_constructionArgs);
            }
        }
        return m_data;
    }

    static inline T* constructObject(Args... args) {
        return new T(std::forward<Args>(args)...);
    }

private:
    std::tuple<Args...> m_constructionArgs;
    std::atomic<T*> m_data;
    std::mutex m_mutex;
};

#endif
