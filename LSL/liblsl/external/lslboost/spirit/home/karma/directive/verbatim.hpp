//  Copyright (c) 2001-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_VERBATIM_MAR_02_2007_0303PM)
#define BOOST_SPIRIT_KARMA_VERBATIM_MAR_02_2007_0303PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <lslboost/spirit/home/karma/meta_compiler.hpp>
#include <lslboost/spirit/home/karma/generator.hpp>
#include <lslboost/spirit/home/karma/domain.hpp>
#include <lslboost/spirit/home/karma/detail/unused_delimiter.hpp>
#include <lslboost/spirit/home/karma/delimit_out.hpp>
#include <lslboost/spirit/home/karma/auxiliary/lazy.hpp>
#include <lslboost/spirit/home/support/unused.hpp>
#include <lslboost/spirit/home/support/common_terminals.hpp>
#include <lslboost/spirit/home/support/has_semantic_action.hpp>
#include <lslboost/spirit/home/support/handles_container.hpp>
#include <lslboost/spirit/home/karma/detail/attributes.hpp>
#include <lslboost/spirit/home/support/info.hpp>

namespace lslboost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_directive<karma::domain, tag::verbatim>   // enables verbatim[]
      : mpl::true_ {};

}}

namespace lslboost { namespace spirit { namespace karma
{
#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
    using spirit::verbatim;
#endif
    using spirit::verbatim_type;

    ///////////////////////////////////////////////////////////////////////////
    //  The verbatim generator is used for verbatim[...] directives.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject>
    struct verbatim_generator : unary_generator<verbatim_generator<Subject> >
    {
        typedef Subject subject_type;
        typedef typename subject_type::properties properties;

        template <typename Context, typename Iterator>
        struct attribute
          : traits::attribute_of<subject_type, Context, Iterator>
        {};

        verbatim_generator(Subject const& subject)
          : subject(subject) {}

        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx, Delimiter const& d
          , Attribute const& attr) const
        {
            //  the verbatim generator simply dispatches to the embedded 
            //  generator while supplying unused_delimiter as the new delimiter
            //  to avoid delimiting down the generator stream
            typedef detail::unused_delimiter<Delimiter> unused_delimiter;

            return subject.generate(sink, ctx, unused_delimiter(d), attr) &&
                   karma::delimit_out(sink, d);     // always do post-delimiting 
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("verbatim", subject.what(context));
        }

        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Modifiers>
    struct make_directive<tag::verbatim, Subject, Modifiers>
    {
        typedef verbatim_generator<Subject> result_type;
        result_type operator()(unused_type, Subject const& subject, unused_type) const
        {
            return result_type(subject);
        }
    };

}}}

namespace lslboost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject>
    struct has_semantic_action<karma::verbatim_generator<Subject> >
      : unary_has_semantic_action<Subject> {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Attribute, typename Context
        , typename Iterator>
    struct handles_container<karma::verbatim_generator<Subject>, Attribute
        , Context, Iterator>
      : unary_handles_container<Subject, Attribute, Context, Iterator> {};
}}}

#endif
