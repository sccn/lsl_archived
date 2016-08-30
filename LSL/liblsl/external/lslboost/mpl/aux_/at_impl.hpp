
#ifndef BOOST_MPL_AUX_AT_IMPL_HPP_INCLUDED
#define BOOST_MPL_AUX_AT_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/mpl for documentation.

// $Id: at_impl.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <lslboost/mpl/begin_end.hpp>
#include <lslboost/mpl/advance.hpp>
#include <lslboost/mpl/deref.hpp>
#include <lslboost/mpl/aux_/traits_lambda_spec.hpp>

namespace lslboost { namespace mpl {

// default implementation; conrete sequences might override it by 
// specializing either the 'at_impl' or the primary 'at' template

template< typename Tag >
struct at_impl
{
    template< typename Sequence, typename N > struct apply
    {
        typedef typename advance<
              typename begin<Sequence>::type
            , N
            >::type iter_;

        typedef typename deref<iter_>::type type;
    };
};

BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(2, at_impl)

}}

#endif // BOOST_MPL_AUX_AT_IMPL_HPP_INCLUDED
