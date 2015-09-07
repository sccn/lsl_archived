/*
  [auto_generated]
  lslboost/numeric/odeint/util/unit_helper.hpp

  [begin_description]
  Get and set the value of a unit.
  [end_description]

  Copyright 2012-2013 Karsten Ahnert
  Copyright 2012-2013 Mario Mulansky

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or
  copy at http://www.lslboost.org/LICENSE_1_0.txt)
*/


#ifndef BOOST_NUMERIC_ODEINT_UTIL_UNIT_HELPER_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_UTIL_UNIT_HELPER_HPP_INCLUDED


#ifndef __CUDACC__
#include <lslboost/units/quantity.hpp>
#include <lslboost/units/get_dimension.hpp>
#include <lslboost/units/get_system.hpp>
#endif



namespace lslboost {
namespace numeric {
namespace odeint {


namespace detail {

    template<class T , class Enabler = void >
    struct get_unit_value_impl
    {
        static T value(const T &t)
        {
            return t;
        }
        typedef T result_type;
    };
    
#ifndef __CUDACC__
    template<class Unit , class T>
    struct get_unit_value_impl< lslboost::units::quantity< Unit , T> >
    {
        static T value( const lslboost::units::quantity< Unit , T> &t )
        {
            return t.value();
        }
        typedef T result_type;
    };
#endif





    template<class T , class V , class Enabler = void >
    struct set_unit_value_impl
    {
        static void set_value(T &t , const V &v)
        {
            t = v;
        }
    };

#ifndef __CUDACC__
    template<class Unit , class T , class V>
    struct set_unit_value_impl<lslboost::units::quantity<Unit , T> , V>
    {
        static void set_value(lslboost::units::quantity<Unit , T> &t , const V &v)
        {
            t = lslboost::units::quantity<Unit , T>::from_value(v);
        }
    };
#endif



} // namespace detail


    template<class T>
    typename detail::get_unit_value_impl<T>::result_type get_unit_value(const T &t)
    {
        return detail::get_unit_value_impl<T>::value(t);
    }


    template<class T , class V>
    void set_unit_value(T &t , const V &v)
    {
        return detail::set_unit_value_impl<T , V>::set_value(t , v);
    }



    template< class T >
    struct unit_value_type
    {
        typedef T type;
    };

#ifndef __CUDACC__
    template< class Unit , class Y >
    struct unit_value_type< lslboost::units::quantity< Unit , Y > >
    {
        typedef Y type;
    };
#endif










    template< typename Time >
    struct inverse_time
    {
        typedef Time type;
    };

#ifndef __CUDACC__
    template< typename Unit , typename Value >
    struct inverse_time< lslboost::units::quantity< Unit , Value > >
    {
        typedef lslboost::units::quantity< Unit , Value > time_type;
        typedef typename lslboost::units::get_dimension< time_type >::type dimension;
        typedef typename lslboost::units::get_system< time_type >::type system;
        typedef typename lslboost::mpl::divides< lslboost::units::dimensionless_type , dimension >::type inv_dimension;
        typedef lslboost::units::unit< inv_dimension , system > inv_unit;
        typedef lslboost::units::quantity< inv_unit , Value > type;
    };
#endif


} // namespace odeint
} // namespace numeric
} // namespace lslboost


#endif // BOOST_NUMERIC_ODEINT_UTIL_UNIT_HELPER_HPP_INCLUDED
