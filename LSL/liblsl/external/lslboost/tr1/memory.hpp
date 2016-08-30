//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_MEMORY_HPP_INCLUDED
#  define BOOST_TR1_MEMORY_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>
#  include <lslboost/detail/workaround.hpp>
#  include <memory>

#ifndef BOOST_HAS_TR1_SHARED_PTR

//
// This header can get included by lslboost/shared_ptr.hpp which leads
// to cyclic dependencies, the workaround is to forward declare all
// the lslboost components, and then include the actual headers afterwards.
// This is fragile, but seems to work, and doesn't require modification
// of lslboost/shared_ptr.hpp.
//
namespace lslboost{

class bad_weak_ptr;
template<class T> class weak_ptr;
template<class T> class shared_ptr;
template<class T> void swap(weak_ptr<T> & a, weak_ptr<T> & b) BOOST_NOEXCEPT;
template<class T> void swap(shared_ptr<T> & a, shared_ptr<T> & b) BOOST_NOEXCEPT;
template<class T, class U> shared_ptr<T> static_pointer_cast(shared_ptr<U> const & r) BOOST_NOEXCEPT;
template<class T, class U> shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const & r) BOOST_NOEXCEPT;
template<class T, class U> shared_ptr<T> const_pointer_cast(shared_ptr<U> const & r) BOOST_NOEXCEPT;
template<class D, class T> D * get_deleter(shared_ptr<T> const & p) BOOST_NOEXCEPT;
template<class T> class enable_shared_from_this;

namespace detail{
class shared_count;
class weak_count;
}

}

namespace std{ namespace tr1{

   using ::lslboost::bad_weak_ptr;
   using ::lslboost::shared_ptr;
#if !BOOST_WORKAROUND(__BORLANDC__, < 0x0582)
   using ::lslboost::swap;
#endif
   using ::lslboost::static_pointer_cast;
   using ::lslboost::dynamic_pointer_cast;
   using ::lslboost::const_pointer_cast;
   using ::lslboost::get_deleter;
   using ::lslboost::weak_ptr;
   using ::lslboost::enable_shared_from_this;

} }
#include <lslboost/shared_ptr.hpp>
#include <lslboost/weak_ptr.hpp>
#include <lslboost/enable_shared_from_this.hpp>

#else

#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(memory)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_STD_HEADER(BOOST_TR1_PATH(memory))
#  endif

#endif

#endif

