/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_VECTOR_ITERATOR_05042005_0635)
#define FUSION_VECTOR_ITERATOR_05042005_0635

#include <lslboost/fusion/support/iterator_base.hpp>
#include <lslboost/fusion/container/vector/detail/deref_impl.hpp>
#include <lslboost/fusion/container/vector/detail/value_of_impl.hpp>
#include <lslboost/fusion/container/vector/detail/next_impl.hpp>
#include <lslboost/fusion/container/vector/detail/prior_impl.hpp>
#include <lslboost/fusion/container/vector/detail/equal_to_impl.hpp>
#include <lslboost/fusion/container/vector/detail/distance_impl.hpp>
#include <lslboost/fusion/container/vector/detail/advance_impl.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/mpl/int.hpp>

namespace lslboost { namespace fusion
{
    struct vector_iterator_tag;
    struct random_access_traversal_tag;

    template <typename Vector, int N>
    struct vector_iterator_identity;

    template <typename Vector, int N>
    struct vector_iterator : iterator_base<vector_iterator<Vector, N> >
    {
        typedef mpl::int_<N> index;
        typedef Vector vector;
        typedef vector_iterator_tag fusion_tag;
        typedef random_access_traversal_tag category;
        typedef vector_iterator_identity<
            typename add_const<Vector>::type, N> identity;

        vector_iterator(Vector& in_vec)
            : vec(in_vec) {}
        Vector& vec;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        vector_iterator& operator= (vector_iterator const&);
    };
}}

#endif

