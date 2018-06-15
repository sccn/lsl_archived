//  Copyright (c) 2001-2011 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_DETAIL_ENABLE_LIT_JAN_06_2011_1009PM)
#define BOOST_SPIRIT_KARMA_DETAIL_ENABLE_LIT_JAN_06_2011_1009PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <lslboost/spirit/home/karma/domain.hpp>
#include <lslboost/spirit/home/karma/generator.hpp>
#include <lslboost/spirit/home/karma/meta_compiler.hpp>
#include <lslboost/spirit/home/karma/auxiliary/lazy.hpp>
#include <lslboost/fusion/include/vector.hpp>
#include <lslboost/spirit/home/support/string_traits.hpp>
#include <lslboost/spirit/home/support/common_terminals.hpp>

namespace lslboost { namespace spirit
{
    // enables lazy lit(...) for karma
    template <>
    struct use_lazy_terminal<karma::domain, tag::lit, 1> 
      : mpl::true_ {};
}}

#endif

