//-----------------------------------------------------------------------------
// lslboost variant/recursive_variant.hpp header file
// See http://www.lslboost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_VARIANT_RECURSIVE_VARIANT_HPP
#define BOOST_VARIANT_RECURSIVE_VARIANT_HPP

#include "lslboost/variant/variant_fwd.hpp"
#include "lslboost/variant/detail/enable_recursive.hpp"
#include "lslboost/variant/detail/substitute_fwd.hpp"
#include "lslboost/variant/detail/make_variant_list.hpp"
#include "lslboost/variant/detail/over_sequence.hpp"

#include "lslboost/mpl/aux_/lambda_arity_param.hpp"

#include "lslboost/mpl/equal.hpp"
#include "lslboost/mpl/eval_if.hpp"
#include "lslboost/mpl/identity.hpp"
#include "lslboost/mpl/if.hpp"
#include "lslboost/mpl/protect.hpp"
#include "lslboost/mpl/transform.hpp"
#include "lslboost/type_traits/is_same.hpp"
#include "lslboost/preprocessor/cat.hpp"
#include "lslboost/preprocessor/repeat.hpp"

#include "lslboost/mpl/bool.hpp"
#include "lslboost/mpl/is_sequence.hpp"
#include "lslboost/variant/variant.hpp"

namespace lslboost {

namespace detail { namespace variant {

///////////////////////////////////////////////////////////////////////////////
// (detail) metafunction specialization substitute
//
// Handles embedded variant types when substituting for recursive_variant_.
//

#if !defined(BOOST_VARIANT_DETAIL_NO_SUBSTITUTE)

template <
      BOOST_VARIANT_ENUM_PARAMS(typename T)
    , typename RecursiveVariant
      BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(typename Arity)
    >
struct substitute<
      ::lslboost::variant<
          recursive_flag< T0 >
        , BOOST_VARIANT_ENUM_SHIFTED_PARAMS(T)
        >
    , RecursiveVariant
    , ::lslboost::recursive_variant_
      BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(Arity)
    >
{
    typedef ::lslboost::variant<
          recursive_flag< T0 >
        , BOOST_VARIANT_ENUM_SHIFTED_PARAMS(T)
        > type;
};

template <
      BOOST_VARIANT_ENUM_PARAMS(typename T)
    , typename RecursiveVariant
      BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(typename Arity)
    >
struct substitute<
      ::lslboost::variant<
          ::lslboost::detail::variant::over_sequence< T0 >
        , BOOST_VARIANT_ENUM_SHIFTED_PARAMS(T)
        >
    , RecursiveVariant
    , ::lslboost::recursive_variant_
      BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(Arity)
    >
{
private:

    typedef T0 initial_types;

    typedef typename mpl::transform<
          initial_types
        , mpl::protect< quoted_enable_recursive<RecursiveVariant,mpl::true_> >
        >::type types;

public:

    typedef typename mpl::if_<
          mpl::equal<initial_types, types, ::lslboost::is_same<mpl::_1, mpl::_2> >
        , ::lslboost::variant<
              ::lslboost::detail::variant::over_sequence< T0 >
            , BOOST_VARIANT_ENUM_SHIFTED_PARAMS(T)
            >
        , ::lslboost::variant< over_sequence<types> >
        >::type type;
};

template <
      BOOST_VARIANT_ENUM_PARAMS(typename T)
    , typename RecursiveVariant
      BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(typename Arity)
    >
struct substitute<
      ::lslboost::variant< BOOST_VARIANT_ENUM_PARAMS(T) >
    , RecursiveVariant
    , ::lslboost::recursive_variant_
      BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(Arity)
    >
{
private: // helpers, for metafunction result (below)

    #define BOOST_VARIANT_AUX_ENABLE_RECURSIVE_TYPEDEFS(z,N,_)  \
        typedef typename enable_recursive<   \
              BOOST_PP_CAT(T,N)              \
            , RecursiveVariant               \
            , mpl::true_                     \
            >::type BOOST_PP_CAT(wknd_T,N);  \
        /**/

    BOOST_PP_REPEAT(
          BOOST_VARIANT_LIMIT_TYPES
        , BOOST_VARIANT_AUX_ENABLE_RECURSIVE_TYPEDEFS
        , _
        )

    #undef BOOST_VARIANT_AUX_ENABLE_RECURSIVE_TYPEDEFS

public: // metafunction result

    typedef ::lslboost::variant< BOOST_VARIANT_ENUM_PARAMS(wknd_T) > type;
};

#else // defined(BOOST_VARIANT_DETAIL_NO_SUBSTITUTE)

//
// no specializations: embedded variants unsupported on these compilers!
//

#endif // !defined(BOOST_VARIANT_DETAIL_NO_SUBSTITUTE)

}} // namespace detail::variant

///////////////////////////////////////////////////////////////////////////////
// metafunction make_recursive_variant
//
// See docs and lslboost/variant/variant_fwd.hpp for more information.
//
template < BOOST_VARIANT_ENUM_PARAMS(typename T) >
struct make_recursive_variant
{
public: // metafunction result

    typedef lslboost::variant<
          detail::variant::recursive_flag< T0 >
        , BOOST_VARIANT_ENUM_SHIFTED_PARAMS(T)
        > type;

};

///////////////////////////////////////////////////////////////////////////////
// metafunction make_recursive_variant_over
//
// See docs and lslboost/variant/variant_fwd.hpp for more information.
//
template <typename Types>
struct make_recursive_variant_over
{
private: // precondition assertions

#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
    BOOST_STATIC_ASSERT(( ::lslboost::mpl::is_sequence<Types>::value ));
#endif

public: // metafunction result

    typedef typename make_recursive_variant<
          detail::variant::over_sequence< Types >
        >::type type;

};

} // namespace lslboost

#endif // BOOST_VARIANT_RECURSIVE_VARIANT_HPP
