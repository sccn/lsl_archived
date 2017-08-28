//////}  // ///////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_CLEAR_ON_DESTRUCTOR_HPP
#define BOOST_INTRUSIVE_DETAIL_CLEAR_ON_DESTRUCTOR_HPP

#include <lslboost/intrusive/detail/config_begin.hpp>

namespace lslboost {
namespace intrusive {
namespace detail {

template<class Derived, bool DoClear = true>
class clear_on_destructor_base
{
   protected:
   ~clear_on_destructor_base()
   {
      static_cast<Derived*>(this)->clear();
   }
};

template<class Derived>
class clear_on_destructor_base<Derived, false>
{};

}  // namespace detail {
}  // namespace intrusive {
}  // namespace lslboost {

#include <lslboost/intrusive/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTRUSIVE_DETAIL_CLEAR_ON_DESTRUCTOR_HPP
