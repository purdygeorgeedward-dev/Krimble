/*
 * SPDX-FileCopyrightText: 2022 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef KIS_XSIMD_GENERIC_DETAILS_DECL_HPP
#define KIS_XSIMD_GENERIC_DETAILS_DECL_HPP

#include <cstdint>
#include <tuple>

namespace xsimd
{

template<typename V, typename T, typename A>
batch<T, A> truncate_to_type(batch<T, A> const &self) noexcept;

template<typename T, typename A>
inline auto set_zero(const batch<T, A> &src, const batch_bool<T, A> &mask) noexcept;

template<typename T, typename A>
inline auto set_one(const batch<T, A> &src, const batch_bool<T, A> &mask) noexcept;

template<typename T, typename T2>
inline T load_and_extend(const T2 *src) noexcept;

template<typename T, typename A = xsimd::current_arch>
inline T *aligned_malloc(size_t size) noexcept;

template<typename T, typename A = xsimd::current_arch>
inline T *vector_aligned_malloc(size_t sz) noexcept;

template<typename T>
inline void vector_aligned_free(const T *ptr) noexcept;

template<typename V>
inline std::pair<V, V> interleave(const V &a, const V &b) noexcept;

template<typename T, typename A>
inline xsimd::batch<T, A> pow2(xsimd::batch<T, A> const &self) noexcept;

namespace kernel
{
namespace detail
{

template<class F, class A, class T>
inline batch<T, A> apply_with_value(F &&func, batch<T, A> const &self) noexcept
{
    alignas(A::alignment()) std::array<T, batch<T, A>::size> self_buffer;
    self.store_aligned(self_buffer.data());
    for (std::size_t i = 0; i < batch<T, A>::size; ++i) {
        self_buffer[i] = func(self_buffer[i]);
    }
    return batch<T, A>::load_aligned(self_buffer.data());
}

template<class F, class A, class T>
inline batch<T, A> apply_with_index_and_value(F &&func, batch<T, A> const &self) noexcept
{
    alignas(A::alignment()) std::array<T, batch<T, A>::size> self_buffer;
    self.store_aligned(self_buffer.data());
    for (std::size_t i = 0; i < batch<T, A>::size; ++i) {
        self_buffer[i] = func(i, self_buffer[i]);
    }
    return batch<T, A>::load_aligned(self_buffer.data());
}
}
}

};

#endif
