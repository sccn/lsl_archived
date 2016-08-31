//  Copyright (c) 2001-2011 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_QI_DETAIL_ENABLE_LIT_JAN_06_2011_0945PM)
#define BOOST_SPIRIT_QI_DETAIL_ENABLE_LIT_JAN_06_2011_0945PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <lslboost/spirit/home/qi/domain.hpp>
#include <lslboost/spirit/home/qi/parser.hpp>
#include <lslboost/spirit/home/qi/meta_compiler.hpp>
#include <lslboost/spirit/home/qi/auxiliary/lazy.hpp>
#include <lslboost/fusion/include/vector.hpp>
#include <lslboost/spirit/home/support/string_traits.hpp>
#include <lslboost/spirit/home/support/common_terminals.hpp>

namespace lslboost { namespace spirit
{
    // enables lazy lit(...) for qi
    template <>
    struct use_lazy_terminal<qi::domain, tag::lit, 1> 
      : mpl::true_ {};
}}

#endif

