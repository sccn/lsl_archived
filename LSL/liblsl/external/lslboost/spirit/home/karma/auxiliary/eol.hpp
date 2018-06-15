//  Copyright (c) 2001-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_EOL_JUL_08_2008_1014AM)
#define BOOST_SPIRIT_KARMA_EOL_JUL_08_2008_1014AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <lslboost/spirit/home/support/common_terminals.hpp>
#include <lslboost/spirit/home/support/info.hpp>
#include <lslboost/spirit/home/support/unused.hpp>
#include <lslboost/spirit/home/karma/detail/attributes.hpp>
#include <lslboost/spirit/home/karma/domain.hpp>
#include <lslboost/spirit/home/karma/meta_compiler.hpp>
#include <lslboost/spirit/home/karma/delimit_out.hpp>
#include <lslboost/spirit/home/karma/detail/generate_to.hpp>

namespace lslboost { namespace spirit 
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_terminal<karma::domain, tag::eol>       // enables eol
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace lslboost { namespace spirit { namespace karma
{
#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
    using lslboost::spirit::eol;
#endif
    using lslboost::spirit::eol_type;

    struct eol_generator : primitive_generator<eol_generator>
    {
        template <typename Context, typename Unused>
        struct attribute
        {
            typedef unused_type type;
        };

        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        static bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& /*attr*/)
        {
            return detail::generate_to(sink, '\n') &&
                   karma::delimit_out(sink, d);   // always do post-delimiting
        }

        template <typename Context>
        info what(Context const& /*context*/) const
        {
            return info("eol");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Modifiers>
    struct make_primitive<tag::eol, Modifiers>
    {
        typedef eol_generator result_type;
        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };

}}}

#endif
