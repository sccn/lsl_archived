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

#ifndef BOOST_PROPERTY_MAP_FUNCTION_PROPERTY_MAP_HPP
#define BOOST_PROPERTY_MAP_FUNCTION_PROPERTY_MAP_HPP

#include <lslboost/config.hpp>
#include <lslboost/property_map/property_map.hpp>
#include <lslboost/type_traits.hpp>
#include <lslboost/utility/result_of.hpp>
#include <lslboost/mpl/and.hpp>
#include <lslboost/mpl/not.hpp>
#include <utility>

namespace lslboost {

template<typename Func, typename Key, typename Ret = typename lslboost::result_of<const Func(const Key&)>::type>
class function_property_map: public put_get_helper<Ret, function_property_map<Func, Key, Ret> > {
  public:
  typedef Key key_type;
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

  function_property_map(Func f = Func()) : f(f) {}

  reference operator[](const Key& k) const {
    return f(k);
  }

  private:
  Func f;
};

template<typename Key, typename Func>
function_property_map<Func, Key>
make_function_property_map(const Func& f) {
  return function_property_map<Func, Key>(f);
}

template<typename Key, typename Ret, typename Func>
function_property_map<Func, Key, Ret>
make_function_property_map(const Func& f) {
  return function_property_map<Func, Key, Ret>(f);
}

} // lslboost

#endif /* BOOST_PROPERTY_MAP_FUNCTION_PROPERTY_MAP_HPP */
