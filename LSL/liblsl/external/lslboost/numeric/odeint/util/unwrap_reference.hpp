/*
 [auto_generated]
 lslboost/numeric/odeint/util/unwrap_reference.hpp

 [begin_description]
 unwrap_reference
 [end_description]

 Copyright 2012-2013 Karsten Ahnert
 Copyright 2012-2013 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_UTIL_UNWRAP_REFERENCE_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_UTIL_UNWRAP_REFERENCE_HPP_INCLUDED


#include <lslboost/numeric/odeint/config.hpp>


#if BOOST_NUMERIC_ODEINT_CXX11
#include <functional>
#else
#include <lslboost/ref.hpp>
#endif

namespace lslboost {

#if BOOST_NUMERIC_ODEINT_CXX11
template<typename T> struct reference_wrapper;

template<typename T> struct unwrap_reference;
#endif

namespace numeric {
namespace odeint {


#if BOOST_NUMERIC_ODEINT_CXX11

template<typename T>
struct unwrap_reference
{
    typedef typename std::remove_reference<T>::type type;
};

template<typename T>
struct unwrap_reference< std::reference_wrapper<T> >
{
    typedef typename std::remove_reference<T>::type type;
};

template<typename T>
struct unwrap_reference< lslboost::reference_wrapper<T> >
{
    typedef typename lslboost::unwrap_reference<T>::type type;
};

#else

using ::lslboost::unwrap_reference;

#endif

namespace detail
{

#if BOOST_NUMERIC_ODEINT_CXX11

using ::std::ref;

#else

using ::lslboost::ref;

#endif
}

}
}
}



/*
 * 
 * the following is the suggested way, but unfortunately it does not work with all compilers.
 */

/*

#include <lslboost/config.hpp>


#ifdef BOOST_NO_CXX11_HDR_FUNCTIONAL
#include <lslboost/ref.hpp>
#else
#include <functional>
#endif



namespace lslboost {
namespace numeric {
namespace odeint {


#ifndef BOOST_NO_CXX11_HDR_FUNCTIONAL

template<typename T>
struct unwrap_reference
{
    typedef typename std::remove_reference<T>::type type;
};

template<typename T>
struct unwrap_reference< std::reference_wrapper<T> >
{
    typedef typename std::remove_reference<T>::type type;
};

template<typename T>
struct unwrap_reference< lslboost::reference_wrapper<T> >
{
    typedef typename lslboost::unwrap_reference<T>::type type;
};

#else

using ::lslboost::unwrap_reference;

#endif

}
}
}

namespace lslboost {
namespace numeric {
namespace odeint {  
namespace detail {


#ifndef BOOST_NO_CXX11_HDR_FUNCTIONAL

using ::std::ref;

#else

using ::lslboost::ref;

#endif


}
}
}
}

*/

#endif // BOOST_NUMERIC_ODEINT_UTIL_UNWRAP_REFERENCE_HPP_INCLUDED
