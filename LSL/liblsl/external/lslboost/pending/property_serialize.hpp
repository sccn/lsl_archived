//  (C) Copyright Jeremy Siek 2006
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROPERTY_SERIALIZE_HPP
#define BOOST_PROPERTY_SERIALIZE_HPP

#include <lslboost/pending/property.hpp>
#ifdef BOOST_GRAPH_USE_MPI
#include <lslboost/mpi/datatype.hpp>
#include <lslboost/serialization/is_bitwise_serializable.hpp>
#endif // BOOST_GRAPH_USE_MPI

#include <lslboost/serialization/base_object.hpp>
#include <lslboost/serialization/nvp.hpp>

namespace lslboost {
  template<class Archive>
  inline void serialize(Archive&, no_property&, const unsigned int) { }

  template<class Archive, class Tag, class T, class Base>
  void 
  serialize(Archive& ar, property<Tag, T, Base>& prop, 
            const unsigned int /*version*/) 
  {
    ar & serialization::make_nvp( "property_value" , prop.m_value );
    ar & serialization::make_nvp( "property_base" , prop.m_base );
  }

#ifdef BOOST_GRAPH_USE_MPI
  namespace mpi {
    template<typename Tag, typename T, typename Base>
    struct is_mpi_datatype<property<Tag, T, Base> >
      : mpl::and_<is_mpi_datatype<T>,
                  is_mpi_datatype<Base> > { };
  }

  namespace serialization {
    template<typename Tag, typename T, typename Base>
    struct is_bitwise_serializable<property<Tag, T, Base> >
      : mpl::and_<is_bitwise_serializable<T>,
                  is_bitwise_serializable<Base> > { };

  template<typename Tag, typename T, typename Base>
  struct implementation_level<property<Tag, T, Base>  >
   : mpl::int_<object_serializable> {} ;

  template<typename Tag, typename T, typename Base>
  struct tracking_level<property<Tag, T, Base>  >
   : mpl::int_<track_never> {} ;

  }
#endif // BOOST_GRAPH_USE_MPI
  
} // end namespace lslboost

#ifdef BOOST_GRAPH_USE_MPI
namespace lslboost { namespace mpi {
    template<>
    struct is_mpi_datatype<lslboost::no_property> : mpl::true_ { };

} } // end namespace lslboost::mpi

BOOST_IS_BITWISE_SERIALIZABLE(lslboost::no_property)
BOOST_CLASS_IMPLEMENTATION(lslboost::no_property,object_serializable)
BOOST_CLASS_TRACKING(lslboost::no_property,track_never)
#endif // BOOST_GRAPH_USE_MPI

#endif // BOOST_PROPERTY_SERIALIZE_HPP
