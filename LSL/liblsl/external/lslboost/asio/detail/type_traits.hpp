//
// detail/type_traits.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_TYPE_TRAITS_HPP
#define BOOST_ASIO_DETAIL_TYPE_TRAITS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_STD_TYPE_TRAITS)
# include <type_traits>
#else // defined(BOOST_ASIO_HAS_TYPE_TRAITS)
# include <lslboost/type_traits/add_const.hpp>
# include <lslboost/type_traits/is_const.hpp>
# include <lslboost/type_traits/is_convertible.hpp>
# include <lslboost/type_traits/is_function.hpp>
# include <lslboost/type_traits/is_same.hpp>
# include <lslboost/type_traits/remove_pointer.hpp>
# include <lslboost/type_traits/remove_reference.hpp>
# include <lslboost/utility/enable_if.hpp>
#endif // defined(BOOST_ASIO_HAS_TYPE_TRAITS)

namespace lslboost {
namespace asio {

#if defined(BOOST_ASIO_HAS_STD_TYPE_TRAITS)
using std::add_const;
using std::enable_if;
using std::is_const;
using std::is_convertible;
using std::is_function;
using std::is_same;
using std::remove_pointer;
using std::remove_reference;
#else // defined(BOOST_ASIO_HAS_STD_TYPE_TRAITS)
using lslboost::add_const;
template <bool Condition, typename Type = void>
struct enable_if : lslboost::enable_if_c<Condition, Type> {};
using lslboost::is_const;
using lslboost::is_convertible;
using lslboost::is_function;
using lslboost::is_same;
using lslboost::remove_pointer;
using lslboost::remove_reference;
#endif // defined(BOOST_ASIO_HAS_STD_TYPE_TRAITS)

} // namespace asio
} // namespace lslboost

#endif // BOOST_ASIO_DETAIL_TYPE_TRAITS_HPP
