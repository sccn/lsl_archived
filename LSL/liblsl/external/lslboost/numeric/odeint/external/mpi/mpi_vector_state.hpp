/*
 [auto_generated]
 lslboost/numeric/odeint/external/mpi/mpi_vector_state.hpp

 [begin_description]
 Copying a container from/to an mpi_state splits/joins it.
 [end_description]

 Copyright 2013 Karsten Ahnert
 Copyright 2013 Mario Mulansky
 Copyright 2013 Pascal Germroth

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_EXTERNAL_MPI_MPI_VECTOR_STATE_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_EXTERNAL_MPI_MPI_VECTOR_STATE_HPP_INCLUDED

#include <vector>
#include <algorithm>
#include <lslboost/mpi.hpp>
#include <lslboost/numeric/odeint/util/copy.hpp>
#include <lslboost/numeric/odeint/util/split_adaptor.hpp>
#include <lslboost/numeric/odeint/algebra/algebra_dispatcher.hpp>
#include <lslboost/numeric/odeint/external/mpi/mpi_state.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {


/** \brief Split data from some container on node 0 to the slaves.
 * Source must be a model of Random Access Range. */
template< class Source , class InnerState >
struct split_impl< Source, mpi_state< InnerState >,
    typename lslboost::enable_if< lslboost::has_range_const_iterator<Source> >::type >
{
    typedef typename lslboost::range_iterator<const Source>::type iterator;

    static void split( const Source &from, mpi_state< InnerState > &to )
    {
        std::vector< InnerState > pieces;
        if(to.world.rank() == 0) {
            const size_t num = static_cast<size_t>(to.world.size());
            pieces.resize(num);
            for(size_t i = 0 ; i < num ; i++) {
                iterator_range<iterator> part = detail::make_split_range(from, i, num);
                lslboost::numeric::odeint::resize(pieces[i], part);
                lslboost::numeric::odeint::copy(part, pieces[i]);
            }
        }
        // send to nodes
        lslboost::mpi::scatter(to.world, pieces, to(), 0);
    }
};

/** \brief Merge data from an mpi_state to some container on node 0.
 * Target must be a model Single Pass Range. */
template< class Target, class InnerState >
struct unsplit_impl< mpi_state< InnerState >, Target,
    typename lslboost::enable_if< lslboost::has_range_iterator<Target> >::type >
{
    typedef typename lslboost::range_iterator<Target>::type iterator;

    static void unsplit( const mpi_state< InnerState > &from , Target &to )
    {
        std::vector< InnerState > pieces;
        // send data to root
        lslboost::mpi::gather(from.world, from(), pieces, 0);
        if(from.world.rank() == 0) {
            // check target size
            size_t total_size = 0;
            for(size_t i = 0 ; i < pieces.size() ; i++)
                total_size += lslboost::size(pieces[i]);
            BOOST_ASSERT( total_size <= lslboost::size(to) );
            // copy parts
            iterator out = lslboost::begin(to);
            for(size_t i = 0 ; i < pieces.size() ; i++)
                out = lslboost::copy(pieces[i], out);
        }
    }
};


}
}
}


#endif

