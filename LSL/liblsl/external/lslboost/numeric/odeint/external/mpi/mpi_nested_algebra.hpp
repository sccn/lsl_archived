/*
 [auto_generated]
 lslboost/numeric/odeint/external/mpi/mpi_nested_algebra.hpp

 [begin_description]
 Nested parallelized algebra for MPI.
 [end_description]

 Copyright 2013 Karsten Ahnert
 Copyright 2013 Mario Mulansky
 Copyright 2013 Pascal Germroth

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_EXTERNAL_MPI_MPI_NESTED_ALGEBRA_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_EXTERNAL_MPI_MPI_NESTED_ALGEBRA_HPP_INCLUDED

#include <lslboost/numeric/odeint/algebra/norm_result_type.hpp>
#include <lslboost/numeric/odeint/util/n_ary_helper.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {

/** \brief MPI-parallelized algebra, wrapping another algebra.
 */
template< class InnerAlgebra >
struct mpi_nested_algebra
{

// execute the InnerAlgebra on each node's local data.
#define BOOST_ODEINT_GEN_BODY(n) \
    InnerAlgebra::for_each##n( \
        BOOST_PP_ENUM_BINARY_PARAMS(n, s, () BOOST_PP_INTERCEPT) , \
        op \
    );
BOOST_ODEINT_GEN_FOR_EACH(BOOST_ODEINT_GEN_BODY)
#undef BOOST_ODEINT_GEN_BODY


    template< class NestedState >
    static typename norm_result_type< typename NestedState::value_type >::type norm_inf( const NestedState &s )
    {
        typedef typename norm_result_type< typename NestedState::value_type >::type result_type;
        // local maximum
        result_type value = InnerAlgebra::norm_inf( s() );
        // global maximum
        return lslboost::mpi::all_reduce(s.world, value, lslboost::mpi::maximum<result_type>());
    }

};


}
}
}

#endif
