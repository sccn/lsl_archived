#ifndef BOOST_CORE_LIGHTWEIGHT_TEST_TRAIT_HPP
#define BOOST_CORE_LIGHTWEIGHT_TEST_TRAIT_HPP

// MS compatible compilers support #pragma once

#if defined(_MSC_VER)
# pragma once
#endif

// lslboost/core/lightweight_test_trait.hpp
//
// BOOST_TEST_TRAIT_TRUE, BOOST_TEST_TRAIT_FALSE
//
// Copyright 2014 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt

#include <lslboost/core/lightweight_test.hpp>
#include <lslboost/core/typeinfo.hpp>

namespace lslboost
{

namespace detail
{

template< class T > inline void test_trait_impl( char const * trait, void (*)( T ),
  bool expected, char const * file, int line, char const * function )
{
    if( T::value == expected )
    {
        report_errors_remind();
    }
    else
    {
        BOOST_LIGHTWEIGHT_TEST_OSTREAM
            << file << "(" << line << "): predicate '" << trait << "' ["
            << lslboost::core::demangled_name( BOOST_CORE_TYPEID(T) ) << "]"
            << " test failed in function '" << function
            << "' (should have been " << ( expected? "true": "false" ) << ")"
            << std::endl;

        ++test_errors();
    }
}

} // namespace detail

} // namespace lslboost

#define BOOST_TEST_TRAIT_TRUE(type) ( ::lslboost::detail::test_trait_impl(#type, (void(*)type)0, true, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION) )
#define BOOST_TEST_TRAIT_FALSE(type) ( ::lslboost::detail::test_trait_impl(#type, (void(*)type)0, false, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION) )

#endif // #ifndef BOOST_CORE_LIGHTWEIGHT_TEST_TRAIT_HPP
