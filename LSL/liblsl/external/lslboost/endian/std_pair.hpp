//  lslboost/endian/std_pair.hpp  ---------------------------------------------------------//

//  Copyright Beman Dawes 2013

//  Distributed under the Boost Software License, Version 1.0.
//  http://www.lslboost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//

#ifndef BOOST_ENDIAN_STD_PAIR_HPP
#define BOOST_ENDIAN_STD_PAIR_HPP

#include <lslboost/endian/conversion.hpp>
#include <utility>

namespace lslboost
{
namespace endian
{
  template <class ReversibleValueT, class ReversibleValueU>
  std::pair<ReversibleValueT, ReversibleValueU>
    reverse_value(std::pair<ReversibleValueT, ReversibleValueU> x)
  {
    return std::pair<ReversibleValueT, ReversibleValueU>(reverse_value(x.first),
      reverse_value(x.second));
  }

  template <class ReversibleT, class ReversibleU>
  void reverse(std::pair<ReversibleT, ReversibleU>& x)
  {
    reverse(x.first);
    reverse(x.second);
  }

}
}

#endif  // BOOST_ENDIAN_STD_PAIR_HPP
