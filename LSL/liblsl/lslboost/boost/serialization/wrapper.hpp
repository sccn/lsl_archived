#ifndef BOOST_SERIALIZATION_WRAPPER_HPP
#define BOOST_SERIALIZATION_WRAPPER_HPP

// (C) Copyright 2005-2006 Matthias Troyer
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/serialization/traits.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/bool_fwd.hpp>

namespace lslboost { namespace serialization {

/// the base class for serialization wrappers
///
/// wrappers need to be treated differently at various places in the serialization library,
/// e.g. saving of non-const wrappers has to be possible. Since partial specialization
// is not supported by all compilers, we derive all wrappers from wrapper_traits. 

template<
    class T, 
    int Level = object_serializable, 
    int Tracking = track_never,
    unsigned int Version = 0,
    class ETII = extended_type_info_impl< T >
>
struct wrapper_traits : 
    public traits<T,Level,Tracking,Version,ETII,mpl::true_> 
{};

template<class T>
struct is_wrapper_impl :
    lslboost::mpl::eval_if<
      lslboost::is_base_and_derived<basic_traits,T>,
      lslboost::mpl::true_,
      lslboost::mpl::false_
    >::type
{};

template<class T>
struct is_wrapper {
    typedef typename is_wrapper_impl<const T>::type type;
};

} // serialization
} // boost

// A macro to define that a class is a wrapper
#define BOOST_CLASS_IS_WRAPPER(T)                       \
namespace lslboost {                                       \
namespace serialization {                               \
template<>                                              \
struct is_wrapper_impl<const T> : lslboost::mpl::true_ {}; \
}                                                       \
}                                                       \
/**/

#endif //BOOST_SERIALIZATION_WRAPPER_HPP
