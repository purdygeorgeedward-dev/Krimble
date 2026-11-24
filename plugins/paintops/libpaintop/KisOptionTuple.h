/*
 *  SPDX-FileCopyrightText: 2022 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISOPTIONTUPLE_H
#define KISOPTIONTUPLE_H

#include <KisMpl.h>
#include <boost/operators.hpp>
#include <boost/tti/has_static_member_data.hpp>

namespace detail {

BOOST_TTI_HAS_STATIC_MEMBER_DATA(supports_prefix)

template <typename T, bool has_member = has_static_member_data_supports_prefix<T, const bool>::value>
struct supports_prefix;

template <typename T>
struct supports_prefix<T, false>
    : std::bool_constant<false>
{
};

template <typename T>
struct supports_prefix<T, true>
    : std::bool_constant<T::supports_prefix>
{
};

template <typename... T>
struct all_support_prefix
{
    static constexpr bool value = (supports_prefix<T>::value && ...);
};

template <typename... T>
struct none_support_prefix
{
    static constexpr bool value = (!supports_prefix<T>::value && ...);
};

template <bool allSupportPrefix, bool noneSupportPrefix, typename... Data>
struct KisOptionTupleImpl
{
    static_assert (allSupportPrefix || noneSupportPrefix,
        "Either **all** or **none** arguments of KisOptionTuple should define 'supports_prefix'");
};

template <typename FirstData, typename... RestData>
struct KisOptionTupleImpl<true, false, FirstData, RestData...>
    : public FirstData,
      public RestData...,
      public boost::equality_comparable<KisOptionTupleImpl<true, false, FirstData, RestData...>>
{
    template <typename... T>
    KisOptionTupleImpl(const QString &prefix, T... args)
        : FirstData(prefix, std::forward<decltype(args)>(args)...),
          RestData(prefix)...
    {
    }

    inline friend bool operator==(const KisOptionTupleImpl<true, false, FirstData, RestData...> &lhs,
                                  const KisOptionTupleImpl<true, false, FirstData, RestData...> &rhs)
    {

        return ((static_cast<const FirstData&>(lhs) == static_cast<const FirstData&>(rhs)) && ... &&
                (static_cast<const RestData&>(lhs) == static_cast<const RestData&>(rhs)));
    }

    bool read(const KisPropertiesConfiguration *setting)
    {
        return (static_cast<FirstData&>(*this).read(setting) && ... &&
                static_cast<RestData&>(*this).read(setting));
    }

    void write(KisPropertiesConfiguration *setting) const
    {
        (static_cast<const FirstData&>(*this).write(setting) , ... ,
                static_cast<const RestData&>(*this).write(setting));
    }

};

template <typename... Data>
struct KisOptionTupleImpl<false, true, Data...>
    : public Data...,
      public boost::equality_comparable<KisOptionTupleImpl<false, true, Data...>>
{
    template <typename... T>
    KisOptionTupleImpl(T... args)
        : kismpl::first_type_t<Data...>(std::forward<decltype(args)>(args)...)
    {
    }

    inline friend bool operator==(const KisOptionTupleImpl<false, true, Data...> &lhs,
                                  const KisOptionTupleImpl<false, true, Data...> &rhs)
    {

        return ((static_cast<const Data&>(lhs) == static_cast<const Data&>(rhs)) && ... );
    }

    bool read(const KisPropertiesConfiguration *setting)
    {
        return (static_cast<Data&>(*this).read(setting) && ... );
    }

    void write(KisPropertiesConfiguration *setting) const
    {
        (static_cast<const Data&>(*this).write(setting) , ... );
    }
};

}

template <typename... Data>
using KisOptionTuple =
    detail::KisOptionTupleImpl<detail::all_support_prefix<Data...>::value,
                               detail::none_support_prefix<Data...>::value,
                               Data...>;

#endif
