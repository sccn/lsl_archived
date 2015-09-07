/*
 [auto_generated]
 lslboost/numeric/odeint/external/mpi/mpi_state.hpp

 [begin_description]
 A generic split state, storing partial data on each node.
 [end_description]

 Copyright 2013 Karsten Ahnert
 Copyright 2013 Mario Mulansky
 Copyright 2013 Pascal Germroth

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_EXTERNAL_MPI_MPI_STATE_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_EXTERNAL_MPI_MPI_STATE_HPP_INCLUDED

#include <vector>
#include <algorithm>
#include <lslboost/mpi.hpp>
#include <lslboost/numeric/odeint/util/copy.hpp>
#include <lslboost/numeric/odeint/util/split.hpp>
#include <lslboost/numeric/odeint/util/resize.hpp>
#include <lslboost/numeric/odeint/util/same_size.hpp>
#include <lslboost/numeric/odeint/algebra/algebra_dispatcher.hpp>
#include <lslboost/numeric/odeint/external/mpi/mpi_nested_algebra.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {

/** \brief A container which has its contents distributed among the nodes.
 */
template< class InnerState >
struct mpi_state
{
    typedef InnerState value_type;

    // the node's local data.
    InnerState m_data;

    lslboost::mpi::communicator world;

    mpi_state() {}
    mpi_state(lslboost::mpi::communicator comm) : world(comm) {}

    inline InnerState &operator()() { return m_data; }
    inline const InnerState &operator()() const { return m_data; }
};




template< class InnerState >
struct is_resizeable< mpi_state< InnerState > >
     : is_resizeable< InnerState > { };


template< class InnerState1 , class InnerState2 >
struct same_size_impl< mpi_state< InnerState1 > , mpi_state< InnerState2 > >
{
    static bool same_size( const mpi_state< InnerState1 > &x , const mpi_state< InnerState2 > &y )
    {
        const bool local = lslboost::numeric::odeint::same_size(x(), y());
        return lslboost::mpi::all_reduce(x.world, local, mpi::bitwise_and<bool>());
    }
};


template< class InnerState1 , class InnerState2 >
struct resize_impl< mpi_state< InnerState1 > , mpi_state< InnerState2 > >
{
    static void resize( mpi_state< InnerState1 > &x , const mpi_state< InnerState2 > &y )
    {
        // resize local parts on each node.
        lslboost::numeric::odeint::resize(x(), y());
    }
};


/** \brief Copy data between mpi_states of same size. */
template< class InnerState1 , class InnerState2 >
struct copy_impl< mpi_state< InnerState1 > , mpi_state< InnerState2 > >
{
    static void copy( const mpi_state< InnerState1 > &from , mpi_state< InnerState2 > &to )
    {
        // copy local parts on each node.
        lslboost::numeric::odeint::copy(from(), to());
    }
};



/** \brief Use `mpi_algebra` for `mpi_state`. */
template< class InnerState >
struct algebra_dispatcher< mpi_state< InnerState > >
{
    typedef mpi_nested_algebra<
        typename algebra_dispatcher< InnerState >::algebra_type
    > algebra_type;
};


}
}
}


#endif
