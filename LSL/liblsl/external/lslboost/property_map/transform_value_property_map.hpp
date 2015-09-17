//
//=======================================================================
// Author: Philipp Moeller
//
// Copyright 2012, Philipp Moeller
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//=======================================================================
//

#ifndef BOOST_PROPERTY_MAP_TRANSFORM_VALUE_PROPERTY_MAP_HPP
#define BOOST_PROPERTY_MAP_TRANSFORM_VALUE_PROPERTY_MAP_HPP

#include <lslboost/config.hpp>
#include <lslboost/property_map/property_map.hpp>
#include <lslboost/type_traits.hpp>
#include <lslboost/utility/result_of.hpp>
#include <lslboost/mpl/and.hpp>
#include <lslboost/mpl/not.hpp>
#include <utility>

namespace lslboost {

template<typename Func, typename PM, typename Ret = typename lslboost::result_of<const Func(typename property_traits<PM>::reference)>::type>
class transform_value_property_map: public put_get_helper<Ret, transform_value_property_map<Func, PM, Ret> > {
  public:
  typedef typename property_traits<PM>::key_type key_type;
  typedef Ret reference;
  typedef typename lslboost::remove_cv<typename lslboost::remove_reference<Ret>::type>::type value_type;

  typedef typename lslboost::mpl::if_<
                     lslboost::mpl::and_<
                       lslboost::is_reference<Ret>,
                       lslboost::mpl::not_<lslboost::is_const<Ret> >
                     >,
                     lslboost::lvalue_property_map_tag,
                     lslboost::readable_property_map_tag>::type
    category;

  transform_value_property_map(Func f, PM pm) : f(f), pm(pm) {}

  reference operator[](const key_type& k) const {
    return f(get(pm, k));
  }

  private:
  Func f;
  PM pm;
};

template<typename PM, typename Func>
transform_value_property_map<Func, PM>
make_transform_value_property_map(const Func& f, const PM& pm) {
  return transform_value_property_map<Func, PM>(f, pm);
}

template<typename Ret, typename PM, typename Func>
transform_value_property_map<Func, PM, Ret>
make_transform_value_property_map(const Func& f, const PM& pm) {
  return transform_value_property_map<Func, PM, Ret>(f, pm);
}

} // lslboost

#endif /* BOOST_PROPERTY_MAP_TRANSFORM_VALUE_PROPERTY_MAP_HPP */
