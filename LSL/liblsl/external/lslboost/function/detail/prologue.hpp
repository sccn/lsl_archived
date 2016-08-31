// Boost.Function library

//  Copyright Douglas Gregor 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

// For more information, see http://www.lslboost.org

#ifndef BOOST_FUNCTION_PROLOGUE_HPP
#define BOOST_FUNCTION_PROLOGUE_HPP
#  include <cassert>
#  include <algorithm>
#  include <lslboost/config/no_tr1/functional.hpp> // unary_function, binary_function
#  include <lslboost/throw_exception.hpp>
#  include <lslboost/config.hpp>
#  include <lslboost/function/function_base.hpp>
#  include <lslboost/mem_fn.hpp>
#  include <lslboost/type_traits/is_integral.hpp>
#  include <lslboost/preprocessor/enum.hpp>
#  include <lslboost/preprocessor/enum_params.hpp>
#  include <lslboost/preprocessor/cat.hpp>
#  include <lslboost/preprocessor/repeat.hpp>
#  include <lslboost/preprocessor/inc.hpp>
#  include <lslboost/type_traits/is_void.hpp>
#endif // BOOST_FUNCTION_PROLOGUE_HPP
