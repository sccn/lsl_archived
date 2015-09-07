/*
 [auto_generated]
 lslboost/numeric/odeint/algebra/range_algebra.hpp

 [begin_description]
 Default algebra, which works with the most state types, like vector< double >, lslboost::array< double >, lslboost::range.
 Internally is uses lslboost::range to obtain the begin and end iterator of the according sequence.
 [end_description]

 Copyright 2010-2013 Karsten Ahnert
 Copyright 2010-2013 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_ALGEBRA_RANGE_ALGEBRA_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_ALGEBRA_RANGE_ALGEBRA_HPP_INCLUDED

#include <lslboost/range.hpp>
#include <lslboost/mpl/size_t.hpp>

#include <lslboost/numeric/odeint/algebra/detail/macros.hpp>
#include <lslboost/numeric/odeint/algebra/detail/for_each.hpp>
#include <lslboost/numeric/odeint/algebra/detail/norm_inf.hpp>
#include <lslboost/numeric/odeint/algebra/norm_result_type.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {

struct range_algebra
{
    template< class S1 , class Op >
    static void for_each1( S1 &s1 , Op op )
    {
        detail::for_each1( lslboost::begin( s1 ) , lslboost::end( s1 ) ,
                op );
    }

    template< class S1 , class S2 , class Op >
    static void for_each2( S1 &s1 , S2 &s2 , Op op )
    {
        detail::for_each2( lslboost::begin( s1 ) , lslboost::end( s1 ) ,
                lslboost::begin( s2 ) , op );
    }

    template< class S1 , class S2 , class S3 , class Op >
    static void for_each3( S1 &s1 , S2 &s2 , S3 &s3 , Op op )
    {
        detail::for_each3( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class Op >
    static void for_each4( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , Op op )
    {
        detail::for_each4( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class Op >
    static void for_each5( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , Op op )
    {
        detail::for_each5( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 , class Op >
    static void for_each6( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , Op op )
    {
        detail::for_each6( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class Op >
    static void for_each7( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , Op op )
    {
        detail::for_each7( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class Op >
    static void for_each8( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , Op op )
    {
        detail::for_each8( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class S9 , class Op >
    static void for_each9( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , S9 &s9 , Op op )
    {
        detail::for_each9( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , lslboost::begin( s9 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class S9 , class S10 , class Op >
    static void for_each10( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , S9 &s9 , S10 &s10 , Op op )
    {
        detail::for_each10( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , lslboost::begin( s9 ) , lslboost::begin( s10 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class S9 , class S10 , class S11 , class Op >
    static void for_each11( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , S9 &s9 , S10 &s10 , S11 &s11 , Op op )
    {
        detail::for_each11( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , lslboost::begin( s9 ) , lslboost::begin( s10 ) , lslboost::begin( s11 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class S9 , class S10 , class S11 , class S12 , class Op >
    static void for_each12( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , S9 &s9 , S10 &s10 , S11 &s11 , S12 &s12 , Op op )
    {
        detail::for_each12( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , lslboost::begin( s9 ) , lslboost::begin( s10 ) , lslboost::begin( s11 ) , lslboost::begin( s12 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class S9 , class S10 , class S11 , class S12 , class S13 , class Op >
    static void for_each13( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , S9 &s9 , S10 &s10 , S11 &s11 , S12 &s12 , S13 &s13 , Op op )
    {
        detail::for_each13( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , lslboost::begin( s9 ) , lslboost::begin( s10 ) , lslboost::begin( s11 ) , lslboost::begin( s12 ) , lslboost::begin( s13 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class S9 , class S10 , class S11 , class S12 , class S13 , class S14 , class Op >
    static void for_each14( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , S9 &s9 , S10 &s10 , S11 &s11 , S12 &s12 , S13 &s13 , S14 &s14 , Op op )
    {
        detail::for_each14( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , lslboost::begin( s9 ) , lslboost::begin( s10 ) , lslboost::begin( s11 ) , lslboost::begin( s12 ) , lslboost::begin( s13 ) , lslboost::begin( s14 ) , op );
    }

    template< class S1 , class S2 , class S3 , class S4 , class S5 , class S6 ,class S7 , class S8 , class S9 , class S10 , class S11 , class S12 , class S13 , class S14 , class S15 , class Op >
    static void for_each15( S1 &s1 , S2 &s2 , S3 &s3 , S4 &s4 , S5 &s5 , S6 &s6 , S7 &s7 , S8 &s8 , S9 &s9 , S10 &s10 , S11 &s11 , S12 &s12 , S13 &s13 , S14 &s14 , S15 &s15 , Op op )
    {
        detail::for_each15( lslboost::begin( s1 ) , lslboost::end( s1 ) , lslboost::begin( s2 ) , lslboost::begin( s3 ) , lslboost::begin( s4 ) , lslboost::begin( s5 ) , lslboost::begin( s6 ) , lslboost::begin( s7 ) , lslboost::begin( s8 ) , lslboost::begin( s9 ) , lslboost::begin( s10 ) , lslboost::begin( s11 ) , lslboost::begin( s12 ) , lslboost::begin( s13 ) , lslboost::begin( s14 ) , lslboost::begin( s15 ) , op );
    }

    template< typename S >
    static typename norm_result_type<S>::type norm_inf( const S &s )
    {
        return detail::norm_inf( lslboost::begin( s ) , lslboost::end( s ) ,
                                 static_cast< typename norm_result_type<S>::type >( 0 ) );
    }

};

} // odeint
} // numeric
} // lslboost


#endif // BOOST_NUMERIC_ODEINT_ALGEBRA_RANGE_ALGEBRA_HPP_INCLUDED
