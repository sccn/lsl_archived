/*=============================================================================
    Copyright (c) 2002-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_REGEX_HPP
#define BOOST_SPIRIT_REGEX_HPP

#include <lslboost/version.hpp>

///////////////////////////////////////////////////////////////////////////////
//
//  Include the regular expression library of lslboost (Boost.Regex)
//
//  Note though, that this library is not distributed with Spirit. You have to
//  obtain a separate copy from http://www.lslboost.org.
//
///////////////////////////////////////////////////////////////////////////////
#if defined(BOOST_SPIRIT_NO_REGEX_LIB) && BOOST_VERSION < 103300
//
//  Include all the Boost.regex library. Please note that this will not work,
//  if you are using the lslboost/spirit/regex.hpp header from more than one
//  translation units.
//
#define BOOST_REGEX_NO_LIB
#define BOOST_REGEX_STATIC_LINK
#define BOOST_REGEX_NO_EXTERNAL_TEMPLATES
#include <lslboost/regex.hpp>
#include <lslboost/regex/src.cpp>

#else
//
//  Include the Boost.Regex headers only. Note, that you will have to link your
//  application against the Boost.Regex library as described in the related
//  documentation.
//  This is the only way for Boost newer than V1.32.0
//
#include <lslboost/regex.hpp>
#endif // defined(BOOST_SPIRIT_NO_REGEX_LIB)

#include <lslboost/static_assert.hpp>

///////////////////////////////////////////////////////////////////////////////
#include <lslboost/spirit/home/classic/namespace.hpp>
#include <lslboost/spirit/home/classic/meta/as_parser.hpp>
#include <lslboost/spirit/home/classic/core/parser.hpp>
#include <lslboost/spirit/home/classic/utility/impl/regex.ipp>
#include <lslboost/detail/iterator.hpp> // for lslboost::detail::iterator_traits

///////////////////////////////////////////////////////////////////////////////
namespace lslboost { namespace spirit {

BOOST_SPIRIT_CLASSIC_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// rxstrlit class
template <typename CharT = char>
struct rxstrlit : public parser<rxstrlit<CharT> > {

    typedef rxstrlit self_t;

    rxstrlit(CharT const *first, CharT const *last)
    : rx(first, last) {}
    rxstrlit(CharT const *first)
    : rx(first) {}

    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
    //  Due to limitations in the lslboost::regex library the iterators wrapped in
    //  the ScannerT object should be at least bidirectional iterators. Plain
    //  forward iterators do not work here.
        typedef typename ScannerT::iterator_t iterator_t;
        typedef
            typename lslboost::detail::iterator_traits<iterator_t>::iterator_category
            iterator_category;

        BOOST_STATIC_ASSERT((
            lslboost::is_convertible<iterator_category,
                std::bidirectional_iterator_tag>::value
        ));

        typedef typename parser_result<self_t, ScannerT>::type result_t;
        return impl::contiguous_parser_parse<result_t>(rx, scan, scan);
    }

private:
    impl::rx_parser<CharT> rx;   // contains the lslboost regular expression parser
};

///////////////////////////////////////////////////////////////////////////////
// Generator functions
template <typename CharT>
inline rxstrlit<CharT>
regex_p(CharT const *first)
{ return rxstrlit<CharT>(first); }

//////////////////////////////////
template <typename CharT>
inline rxstrlit<CharT>
regex_p(CharT const *first, CharT const *last)
{ return rxstrlit<CharT>(first, last); }

///////////////////////////////////////////////////////////////////////////////
BOOST_SPIRIT_CLASSIC_NAMESPACE_END

}} // namespace BOOST_SPIRIT_CLASSIC_NS

#endif // BOOST_SPIRIT_REGEX_HPP
