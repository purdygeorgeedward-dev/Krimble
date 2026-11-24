/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISMPL_H
#define KISMPL_H

#include <tuple>
#include <utility>
#include <optional>
#include <functional>

namespace kismpl {

namespace detail {

template<std::size_t ...Idx>
struct make_index_sequence_from_1_impl;

template<std::size_t num, std::size_t ...Idx>
struct make_index_sequence_from_1_impl<num, Idx...> {
    using type = typename make_index_sequence_from_1_impl<num - 1, num, Idx...>::type;
};

template<std::size_t ...Idx>
struct make_index_sequence_from_1_impl<0, Idx...> {
    using type = std::index_sequence<Idx...>;
};

}

template<std::size_t Num>
using make_index_sequence_from_1 =
    typename detail::make_index_sequence_from_1_impl<Num>::type;

namespace detail {

template <typename F, typename Tuple, std::size_t... I>
auto apply_to_tuple_impl(F f, Tuple &&t, std::index_sequence<I...>) {
    return std::make_tuple(f(std::get<I>(std::forward<Tuple>(t)))...);
}
}

template <typename F, typename Tuple>
auto apply_to_tuple(F f, Tuple &&t) {
    return detail::apply_to_tuple_impl(std::forward<F>(f), std::forward<Tuple>(t),
                                       std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}

constexpr auto unzip_wrapper = [] (auto f) {
    return
        [=] (auto &&x) {
            return std::apply(f, std::forward<decltype(x)>(x));
        };
};

namespace detail {

template <typename First, typename... Rest>
struct first_type_impl
{
    using type = First;
};

}

template <typename... T>
struct first_type
{
    using type = typename detail::first_type_impl<T...>::type;
};

template <typename... T>
using first_type_t = typename first_type<T...>::type;

namespace detail {
template <typename Fun, typename T>
struct fold_optional_impl {
    std::optional<T> fold(const std::optional<T> &first) {
        return first;
    }

    std::optional<T> fold(const std::optional<T> &first, const std::optional<T> &second) {
        if (first && second) {
            return m_fun(*first, *second);
        } else if (first) {
            return first;
        } else {
            return second;
        }
    }

    template <typename... Rest>
    std::optional<T> fold(const std::optional<T> &first, std::optional<T> const &second, const std::optional<Rest> &...rest) {
        return fold(fold(first, second), rest...);
    }

    const Fun m_fun;
};

}

template <typename Fun, typename... Args,
         typename T = typename first_type_t<std::remove_reference_t<Args>...>::value_type>
std::optional<T> fold_optional(Fun &&fun, Args &&...args) {
    return detail::fold_optional_impl<Fun, T>{std::forward<Fun>(fun)}.fold(args...);
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace detail {

template<typename Op, typename Class, typename MemType, typename PtrType>
struct mem_checker
{
    template <typename Object>
    bool operator() (Object &&obj) const {
        Op op;
        return op(std::invoke(ptr, std::forward<Object>(obj)), value);
    }

    PtrType ptr;
    const MemType value;
};

template<typename Op, typename Class, typename MemType, typename PtrType>
struct mem_compare
{
    template <typename Object>
    auto operator() (Object &&lhs, Object &&rhs) const {
        Op op;
        return op(std::invoke(ptr, std::forward<Object>(lhs)),
                  std::invoke(ptr, std::forward<Object>(rhs)));
    }

    template <typename Object>
    auto operator() (Object &&lhs, const MemType &rhs) const {
        Op op;
        return op(std::invoke(ptr, std::forward<Object>(lhs)),
                  rhs);
    }

    template <typename Object>
    auto operator() (const MemType &lhs, Object &&rhs) const {
        Op op;
        return op(lhs,
                  std::invoke(ptr, std::forward<Object>(rhs)));
    }

    PtrType ptr;
};

}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_equal_to(MemTypeNoRef Class::*ptr, MemType &&value) {
    return detail::mem_checker<std::equal_to<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_equal_to(MemTypeNoRef (Class::*ptr)(), MemType &&value) {
    return detail::mem_checker<std::equal_to<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_equal_to(MemTypeNoRef (Class::*ptr)() const, MemType &&value) {
    return detail::mem_checker<std::equal_to<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_equal_to(MemTypeNoRef (Class::*ptr)() noexcept, MemType &&value) {
    return detail::mem_checker<std::equal_to<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_equal_to(MemTypeNoRef (Class::*ptr)() const noexcept, MemType &&value) {
    return detail::mem_checker<std::equal_to<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less(MemTypeNoRef Class::*ptr, MemType &&value) {
    return detail::mem_checker<std::less<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less(MemTypeNoRef (Class::*ptr)(), MemType &&value) {
    return detail::mem_checker<std::less<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less(MemTypeNoRef (Class::*ptr)() const, MemType &&value) {
    return detail::mem_checker<std::less<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less(MemTypeNoRef (Class::*ptr)() noexcept, MemType &&value) {
    return detail::mem_checker<std::less<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less(MemTypeNoRef (Class::*ptr)() const noexcept, MemType &&value) {
    return detail::mem_checker<std::less<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less_equal(MemTypeNoRef Class::*ptr, MemType &&value) {
    return detail::mem_checker<std::less_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less_equal(MemTypeNoRef (Class::*ptr)(), MemType &&value) {
    return detail::mem_checker<std::less_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less_equal(MemTypeNoRef (Class::*ptr)() const, MemType &&value) {
    return detail::mem_checker<std::less_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less_equal(MemTypeNoRef (Class::*ptr)() noexcept, MemType &&value) {
    return detail::mem_checker<std::less_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_less_equal(MemTypeNoRef (Class::*ptr)() const noexcept, MemType &&value) {
    return detail::mem_checker<std::less_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater(MemTypeNoRef Class::*ptr, MemType &&value) {
    return detail::mem_checker<std::greater<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater(MemTypeNoRef (Class::*ptr)(), MemType &&value) {
    return detail::mem_checker<std::greater<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater(MemTypeNoRef (Class::*ptr)() const, MemType &&value) {
    return detail::mem_checker<std::greater<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater(MemTypeNoRef (Class::*ptr)() noexcept, MemType &&value) {
    return detail::mem_checker<std::greater<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater(MemTypeNoRef (Class::*ptr)() const noexcept, MemType &&value) {
    return detail::mem_checker<std::greater<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater_equal(MemTypeNoRef Class::*ptr, MemType &&value) {
    return detail::mem_checker<std::greater_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater_equal(MemTypeNoRef (Class::*ptr)(), MemType &&value) {
    return detail::mem_checker<std::greater_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater_equal(MemTypeNoRef (Class::*ptr)() const, MemType &&value) {
    return detail::mem_checker<std::greater_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater_equal(MemTypeNoRef (Class::*ptr)() noexcept, MemType &&value) {
    return detail::mem_checker<std::greater_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType, typename MemTypeNoRef = std::remove_reference_t<MemType>>
inline auto mem_greater_equal(MemTypeNoRef (Class::*ptr)() const noexcept, MemType &&value) {
    return detail::mem_checker<std::greater_equal<>, Class, MemTypeNoRef, decltype(ptr)>{ptr, std::forward<MemType>(value)};
}

template<typename Class, typename MemType>
inline auto mem_less(MemType Class::*ptr) {
    return detail::mem_compare<std::less<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::less<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less(MemType (Class::*ptr)() const) {
    return detail::mem_compare<std::less<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less(MemType (Class::*ptr)() noexcept) {
    return detail::mem_compare<std::less<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less(MemType (Class::*ptr)() const noexcept) {
    return detail::mem_compare<std::less<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less_equal(MemType Class::*ptr) {
    return detail::mem_compare<std::less_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less_equal(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::less_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less_equal(MemType (Class::*ptr)() const) {
    return detail::mem_compare<std::less_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less_equal(MemType (Class::*ptr)() noexcept) {
    return detail::mem_compare<std::less_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_less_equal(MemType (Class::*ptr)() const noexcept) {
    return detail::mem_compare<std::less_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater(MemType Class::*ptr) {
    return detail::mem_compare<std::greater<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::greater<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater(MemType (Class::*ptr)() const) {
    return detail::mem_compare<std::greater<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater(MemType (Class::*ptr)() noexcept) {
    return detail::mem_compare<std::greater<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater(MemType (Class::*ptr)() const noexcept) {
    return detail::mem_compare<std::greater<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater_equal(MemType Class::*ptr) {
    return detail::mem_compare<std::greater_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater_equal(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::greater_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater_equal(MemType (Class::*ptr)() const) {
    return detail::mem_compare<std::greater_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater_equal(MemType (Class::*ptr)() noexcept) {
    return detail::mem_compare<std::greater_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_greater_equal(MemType (Class::*ptr)() const noexcept) {
    return detail::mem_compare<std::greater_equal<>, Class, MemType, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_bit_or(MemType Class::*ptr) {
    return detail::mem_compare<std::bit_or<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_bit_or(MemType (Class::*ptr)() const) {
    return detail::mem_compare<std::bit_or<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_bit_and(MemType Class::*ptr) {
    return detail::mem_compare<std::bit_and<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_bit_and(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::bit_and<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_bit_xor(MemType Class::*ptr) {
    return detail::mem_compare<std::bit_xor<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_bit_xor(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::bit_xor<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_plus(MemType Class::*ptr) {
    return detail::mem_compare<std::plus<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_plus(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::plus<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_minus(MemType Class::*ptr) {
    return detail::mem_compare<std::minus<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_minus(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::minus<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_multiplies(MemType Class::*ptr) {
    return detail::mem_compare<std::multiplies<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_multiplies(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::multiplies<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_divides(MemType Class::*ptr) {
    return detail::mem_compare<std::divides<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template<typename Class, typename MemType>
inline auto mem_divides(MemType (Class::*ptr)()) {
    return detail::mem_compare<std::divides<>, Class, std::invoke_result_t<decltype(ptr), Class>, decltype(ptr)>{ptr};
}

template <typename F>
struct finally {
    finally(F &&f)
        : m_f(std::forward<F>(f))
    {
    }

    finally(const finally &) = delete;
    finally(finally &&) = delete;

    ~finally() {
        m_f();
    }
private:
    F m_f;
};

}

#endif
