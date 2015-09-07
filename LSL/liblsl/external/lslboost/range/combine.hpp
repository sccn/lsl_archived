//  Copyright Neil Groves 2010. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_COMBINE_HPP
#define BOOST_RANGE_COMBINE_HPP

#include <lslboost/config.hpp>
#include <lslboost/range/iterator_range_core.hpp>
#include <lslboost/iterator/zip_iterator.hpp>

namespace lslboost
{
    namespace range
    {

template<typename IterTuple>
class combined_range
        : public iterator_range<zip_iterator<IterTuple> >
{
    typedef iterator_range<zip_iterator<IterTuple> > base;
public:
    combined_range(IterTuple first, IterTuple last)
        : base(first, last)
    {
    }
};

    } // namespace range
} // namespace lslboost

#if defined(BOOST_NO_CXX11_AUTO_DECLARATIONS) || \
    defined(BOOST_NO_CXX11_DECLTYPE) || \
    defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || \
    defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#   include <lslboost/range/detail/combine_cxx03.hpp>
#else
#   include <lslboost/range/detail/combine_cxx11.hpp>
#endif

#endif
