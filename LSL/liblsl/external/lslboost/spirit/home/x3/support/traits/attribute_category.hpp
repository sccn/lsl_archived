/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ATTRIBUTE_CATEGORY_JAN_4_2012_1150AM)
#define BOOST_SPIRIT_X3_ATTRIBUTE_CATEGORY_JAN_4_2012_1150AM

#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/logical.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/fusion/include/copy.hpp>
#include <lslboost/fusion/include/is_sequence.hpp>
#include <lslboost/fusion/support/category_of.hpp>
#include <lslboost/spirit/home/x3/support/traits/is_variant.hpp>
#include <lslboost/spirit/home/x3/support/traits/container_traits.hpp>

namespace lslboost { namespace spirit { namespace x3
{
   struct unused_type;
}}}

namespace lslboost { namespace spirit { namespace x3 { namespace traits
{
    struct unused_attribute {};
    struct plain_attribute {};
    struct container_attribute {};
    struct tuple_attribute {};
    struct associative_attribute {};
    struct variant_attribute {};
    struct optional_attribute {};

    template <typename T, typename Enable = void>
    struct attribute_category
        : mpl::identity<plain_attribute> {};

    template <>
    struct attribute_category<unused_type>
        : mpl::identity<unused_attribute> {};

    template <>
    struct attribute_category<unused_type const>
        : mpl::identity<unused_attribute> {};

    template <typename T>
    struct attribute_category< T
	, typename enable_if<
	      typename mpl::eval_if< 
		  fusion::traits::is_sequence<T>
		  , fusion::traits::is_associative<T>
		  , mpl::false_
		  >::type >::type >
        : mpl::identity<associative_attribute> {};

    template <typename T>
    struct attribute_category< T
	, typename enable_if<
	      mpl::and_<
		  fusion::traits::is_sequence<T>
		  , mpl::not_<fusion::traits::is_associative<T> > 
		  > >::type >
        : mpl::identity<tuple_attribute> {};

    template <typename T>
    struct attribute_category<T,
        typename enable_if<traits::is_variant<T>>::type>
        : mpl::identity<variant_attribute> {};

    template <typename T>
    struct attribute_category<T,
        typename enable_if<traits::is_container<T>>::type>
        : mpl::identity<container_attribute> {};

}}}}

#endif
