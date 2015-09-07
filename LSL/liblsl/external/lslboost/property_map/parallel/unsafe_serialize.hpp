// Copyright (C) 2006 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  Authors: Douglas Gregor
//           Andrew Lumsdaine

// This file contains the "unsafe_serialize" routine, which transforms
// types they may not be serializable (such as void*) into
// serializable equivalents.
#ifndef BOOST_PROPERTY_MAP_UNSAFE_SERIALIZE_HPP
#define BOOST_PROPERTY_MAP_UNSAFE_SERIALIZE_HPP

#include <lslboost/mpi/datatype.hpp>
#include <lslboost/serialization/is_bitwise_serializable.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/cstdint.hpp>
#include <lslboost/static_assert.hpp>
#include <lslboost/type_traits.hpp>
#include <utility>

BOOST_IS_BITWISE_SERIALIZABLE(void*)
namespace lslboost { namespace mpi {
    template<> struct is_mpi_datatype<void*> : mpl::true_ { };
} } // end namespace lslboost::mpi

namespace lslboost {
  typedef mpl::if_c<(sizeof(int) == sizeof(void*)), 
                    int, 
                    mpl::if_c<(sizeof(long) == sizeof(void*)),
                              long,
                              mpl::if_c<(sizeof(void*) <= sizeof(lslboost::intmax_t)),
                                        lslboost::intmax_t,
                                        void>::type
                              >::type
                    >::type ptr_serialize_type;

  BOOST_STATIC_ASSERT ((!lslboost::is_void<ptr_serialize_type>::value));
    
  template<typename T> inline T& unsafe_serialize(T& x) { return x; }

  inline ptr_serialize_type& unsafe_serialize(void*& x)
  { return reinterpret_cast<ptr_serialize_type&>(x); }

  // Force Boost.MPI to serialize a void* like a ptr_serialize_type
  namespace mpi {
    template<> inline MPI_Datatype get_mpi_datatype<void*>(void* const& x)
    {
      return get_mpi_datatype<ptr_serialize_type>();
    }
  }

  template<typename T, typename U>
  struct unsafe_pair
  {
    unsafe_pair() { }
    unsafe_pair(const T& t, const U& u) : first(t), second(u) { }
    unsafe_pair(const std::pair<T, U>& p) : first(p.first), second(p.second) { }
    T first;
    U second;

    template<typename Archiver>
    void serialize(Archiver& ar, const unsigned /*version*/)
    {
      ar & unsafe_serialize(first) & unsafe_serialize(second);
    }
  };

  template<typename T, typename U>
  bool operator<(unsafe_pair<T,U> const& x, unsafe_pair<T,U> const& y)
  {
    return std::make_pair(x.first, x.second) < 
      std::make_pair(y.first, y.second);  
  }

} // end namespace lslboost

#endif // BOOST_PROPERTY_MAP_UNSAFE_SERIALIZE_HPP
