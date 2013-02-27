// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
#ifndef BOOST_CONCEPT_CHECK_BORLAND_DWA2006429_HPP
# define BOOST_CONCEPT_CHECK_BORLAND_DWA2006429_HPP

namespace lslboost {

template <class ModelFn>
struct concept_check;

template <class Model>
struct concept_check<void(*)(Model)>
{
    enum { instantiate = sizeof((((Model*)0)->~Model()), 3) };
};

#  define BOOST_CONCEPT_ASSERT( ModelInParens )                     \
  enum { BOOST_PP_CAT(lslboost_concept_check,__LINE__) =               \
         lslboost::concept_check<void(*)ModelInParens>::instantiate    \
  }

} // namespace lslboost::concept_checking

#endif // BOOST_CONCEPT_CHECK_BORLAND_DWA2006429_HPP
