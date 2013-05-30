//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_TOOLS_SOLVE_HPP
#define BOOST_MATH_TOOLS_SOLVE_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <lslboost/config.hpp>
#include <lslboost/assert.hpp>

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4996 4267 4244)
#endif

#include <lslboost/numeric/ublas/lu.hpp>
#include <lslboost/numeric/ublas/matrix.hpp>
#include <lslboost/numeric/ublas/vector.hpp>

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

namespace lslboost{ namespace math{ namespace tools{

//
// Find x such that Ax = b
//
// Caution: this uses undocumented, and untested ublas code,
// however short of writing our own LU-decompostion code
// it's the only game in town.
//
template <class T>
lslboost::numeric::ublas::vector<T> solve(
          const lslboost::numeric::ublas::matrix<T>& A_,
          const lslboost::numeric::ublas::vector<T>& b_)
{
   //BOOST_ASSERT(A_.size() == b_.size());

   lslboost::numeric::ublas::matrix<T> A(A_);
   lslboost::numeric::ublas::vector<T> b(b_);
   lslboost::numeric::ublas::permutation_matrix<> piv(b.size());
   lu_factorize(A, piv);
   lu_substitute(A, piv, b);
   //
   // iterate to reduce error:
   //
   lslboost::numeric::ublas::vector<T> delta(b.size());
   for(unsigned i = 0; i < 1; ++i)
   {
      noalias(delta) = prod(A_, b);
      delta -= b_;
      lu_substitute(A, piv, delta);
      b -= delta;

      T max_error = 0;

      for(unsigned i = 0; i < delta.size(); ++i)
      {
         T err = fabs(delta[i] / b[i]);
         if(err > max_error)
            max_error = err;
      }
      //std::cout << "Max change in LU error correction: " << max_error << std::endl;
   }

   return b;
}

}}} // namespaces

#endif // BOOST_MATH_TOOLS_SOLVE_HPP


