//-----------------------------------------------------------------------------
// lslboost variant/detail/enable_recursive.hpp header file
// See http://www.lslboost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_VARIANT_DETAIL_ENABLE_RECURSIVE_HPP
#define BOOST_VARIANT_DETAIL_ENABLE_RECURSIVE_HPP

#include "lslboost/variant/detail/enable_recursive_fwd.hpp"
#include "lslboost/variant/variant_fwd.hpp"

#if !defined(BOOST_VARIANT_NO_FULL_RECURSIVE_VARIANT_SUPPORT)
#   include "lslboost/mpl/apply.hpp"
#   include "lslboost/mpl/eval_if.hpp"
#   include "lslboost/mpl/lambda.hpp"
#endif

#include "lslboost/variant/detail/substitute.hpp"
#include "lslboost/mpl/aux_/config/ctps.hpp"
#include "lslboost/mpl/bool_fwd.hpp"
#include "lslboost/mpl/if.hpp"
#include "lslboost/mpl/or.hpp"
#include "lslboost/type_traits/is_pointer.hpp"
#include "lslboost/type_traits/is_reference.hpp"
#include "lslboost/type_traits/is_same.hpp"

#include "lslboost/variant/recursive_wrapper.hpp"

namespace lslboost {
namespace detail { namespace variant {

#if !defined(BOOST_VARIANT_DETAIL_NO_SUBSTITUTE)

#   define BOOST_VARIANT_AUX_ENABLE_RECURSIVE_IMPL(T,Dest,Source) \
    substitute< T , Dest , Source > \
    /**/

#else // defined(BOOST_VARIANT_DETAIL_NO_SUBSTITUTE)

///////////////////////////////////////////////////////////////////////////////
// (detail) class template rebind1
//
// Limited workaround in case 'substitute' metafunction unavailable.
//

template <typename T, typename U1>
struct rebind1
{
private:
    typedef typename mpl::lambda<
          mpl::identity<T>
        >::type le_;

public:
    typedef typename mpl::eval_if<
          is_same< le_, mpl::identity<T> >
        , le_ // identity<T>
        , mpl::apply1<le_, U1>
        >::type type;
};

#   define BOOST_VARIANT_AUX_ENABLE_RECURSIVE_IMPL(T,Dest,Source) \
    rebind1< T , Dest > \
    /**/

#endif // !defined(BOOST_VARIANT_DETAIL_NO_SUBSTITUTE)

///////////////////////////////////////////////////////////////////////////////
// (detail) metafunction enable_recursive
//
// See lslboost/variant/detail/enable_recursive_fwd.hpp for more information.
//

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template <typename T, typename RecursiveVariant, typename NoWrapper>
struct enable_recursive
    : BOOST_VARIANT_AUX_ENABLE_RECURSIVE_IMPL(
          T, RecursiveVariant, ::lslboost::recursive_variant_
        )
{
};

template <typename T, typename RecursiveVariant>
struct enable_recursive< T,RecursiveVariant,mpl::false_ >
{
private: // helpers, for metafunction result (below)

    typedef typename BOOST_VARIANT_AUX_ENABLE_RECURSIVE_IMPL(
          T, RecursiveVariant, ::lslboost::recursive_variant_
        )::type t_;

public: // metafunction result

    // [Wrap with recursive_wrapper only if rebind really changed something:]
    typedef typename mpl::if_<
          mpl::or_<
              is_same< t_,T >
            , is_reference<t_>
            , is_pointer<t_>
            >
        , t_
        , lslboost::recursive_wrapper<t_>
        >::type type;

};

#else // defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template <typename T, typename RecursiveVariant, typename NoWrapper>
struct enable_recursive
{
private: // helpers, for metafunction result (below)

    typedef typename BOOST_VARIANT_AUX_ENABLE_RECURSIVE_IMPL(
          T, RecursiveVariant, ::lslboost::recursive_variant_
        )::type t_;

public: // metafunction result

    // [Wrap with recursive_wrapper only if rebind really changed something:]
    typedef typename mpl::if_<
          mpl::or_<
              NoWrapper
            , is_same< t_,T >
            , is_reference<t_>
            , is_pointer<t_>
            >
        , t_
        , lslboost::recursive_wrapper<t_>
        >::type type;

};

#endif // BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION workaround

///////////////////////////////////////////////////////////////////////////////
// (detail) metafunction class quoted_enable_recursive
//
// Same behavior as enable_recursive metafunction (see above).
//
template <typename RecursiveVariant, typename NoWrapper>
struct quoted_enable_recursive
{
    template <typename T>
    struct apply
        : enable_recursive<T, RecursiveVariant, NoWrapper>
    {
    };
};

}} // namespace detail::variant
} // namespace lslboost

#endif // BOOST_VARIANT_DETAIL_ENABLE_RECURSIVE_HPP
