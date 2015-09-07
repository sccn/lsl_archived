/*
 [auto_generated]
 lslboost/numeric/odeint/external/compute/compute_resize.hpp

 [begin_description]
 Enable resizing for Boost.Compute vector
 [end_description]

 Copyright 2009-2011 Karsten Ahnert
 Copyright 2009-2011 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_EXTERNAL_COMPUTE_COMPUTE_RESIZE_HPP_DEFINED
#define BOOST_NUMERIC_ODEINT_EXTERNAL_COMPUTE_COMPUTE_RESIZE_HPP_DEFINED

#include <lslboost/compute/container/vector.hpp>

#include <lslboost/numeric/odeint/util/copy.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {

template< class T, class A >
struct is_resizeable< lslboost::compute::vector< T , A > >
{
    struct type : public lslboost::true_type { };
    const static bool value = type::value;
};

template< class T, class A >
struct same_size_impl< lslboost::compute::vector< T, A > , lslboost::compute::vector< T, A > >
{
    static bool same_size( const lslboost::compute::vector< T, A > &x , const lslboost::compute::vector< T, A > &y )
    {
        return x.size() == y.size();
    }
};

template< class T, class A >
struct resize_impl< lslboost::compute::vector< T, A > , lslboost::compute::vector< T, A > >
{
    static void resize( lslboost::compute::vector< T, A > &x , const lslboost::compute::vector< T, A > &y )
    {
        x.resize( y.size() );
    }
};


template< class Container1, class T, class A >
struct copy_impl< Container1 , lslboost::compute::vector< T, A > >
{
    static void copy( const Container1 &from , lslboost::compute::vector< T, A > &to )
    {
        lslboost::compute::copy( lslboost::begin( from ) , lslboost::end( from ) , lslboost::begin( to ) );
    }
};

template< class T, class A, class Container2 >
struct copy_impl< lslboost::compute::vector< T, A > , Container2 >
{
    static void copy( const lslboost::compute::vector< T, A > &from , Container2 &to )
    {
        lslboost::compute::copy( lslboost::begin( from ) , lslboost::end( from ) , lslboost::begin( to ) );
    }
};

template< class T, class A >
struct copy_impl< lslboost::compute::vector< T, A > , lslboost::compute::vector< T, A > >
{
    static void copy( const lslboost::compute::vector< T, A > &from , lslboost::compute::vector< T, A > &to )
    {
        lslboost::compute::copy( lslboost::begin( from ) , lslboost::end( from ) , lslboost::begin( to ) );
    }
};




} // odeint
} // numeric
} // lslboost


#endif // BOOST_NUMERIC_ODEINT_EXTERNAL_COMPUTE_COMPUTE_RESIZE_HPP_DEFINED
