///////////////////////////////////////////////////////////////////////////////
/// \file algorithm.hpp
///   Includes the range-based versions of the algorithms in the
///   C++ standard header file <algorithm>
//
/////////////////////////////////////////////////////////////////////////////

// Copyright 2009 Neil Groves.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// Acknowledgements:
// This code uses combinations of ideas, techniques and code snippets
// from: Thorsten Ottosen, Eric Niebler, Jeremy Siek,
// and Vladimir Prus'
//
// The original mutating algorithms that served as the first version
// were originally written by Vladimir Prus'
// <ghost@cs.msu.su> code from Boost Wiki

#if defined(_MSC_VER) && _MSC_VER >= 1000
#pragma once
#endif

#ifndef BOOST_RANGE_ALGORITHM_HPP_INCLUDED_01012009
#define BOOST_RANGE_ALGORITHM_HPP_INCLUDED_01012009

#include <lslboost/range/concepts.hpp>
#include <lslboost/range/iterator_range.hpp>
#include <lslboost/range/difference_type.hpp>
#include <lslboost/range/detail/range_return.hpp>
#include <lslboost/iterator/iterator_traits.hpp>
#include <lslboost/next_prior.hpp>
#include <algorithm>

// Non-mutating algorithms
#include <lslboost/range/algorithm/adjacent_find.hpp>
#include <lslboost/range/algorithm/count.hpp>
#include <lslboost/range/algorithm/count_if.hpp>
#include <lslboost/range/algorithm/equal.hpp>
#include <lslboost/range/algorithm/for_each.hpp>
#include <lslboost/range/algorithm/find.hpp>
#include <lslboost/range/algorithm/find_end.hpp>
#include <lslboost/range/algorithm/find_first_of.hpp>
#include <lslboost/range/algorithm/find_if.hpp>
#include <lslboost/range/algorithm/lexicographical_compare.hpp>
#include <lslboost/range/algorithm/mismatch.hpp>
#include <lslboost/range/algorithm/search.hpp>
#include <lslboost/range/algorithm/search_n.hpp>

// Mutating algorithms
#include <lslboost/range/algorithm/copy.hpp>
#include <lslboost/range/algorithm/copy_backward.hpp>
#include <lslboost/range/algorithm/fill.hpp>
#include <lslboost/range/algorithm/fill_n.hpp>
#include <lslboost/range/algorithm/generate.hpp>
#include <lslboost/range/algorithm/inplace_merge.hpp>
#include <lslboost/range/algorithm/merge.hpp>
#include <lslboost/range/algorithm/nth_element.hpp>
#include <lslboost/range/algorithm/partial_sort.hpp>
#include <lslboost/range/algorithm/partial_sort_copy.hpp>
#include <lslboost/range/algorithm/partition.hpp>
#include <lslboost/range/algorithm/random_shuffle.hpp>
#include <lslboost/range/algorithm/remove.hpp>
#include <lslboost/range/algorithm/remove_copy.hpp>
#include <lslboost/range/algorithm/remove_copy_if.hpp>
#include <lslboost/range/algorithm/remove_if.hpp>
#include <lslboost/range/algorithm/replace.hpp>
#include <lslboost/range/algorithm/replace_copy.hpp>
#include <lslboost/range/algorithm/replace_copy_if.hpp>
#include <lslboost/range/algorithm/replace_if.hpp>
#include <lslboost/range/algorithm/reverse.hpp>
#include <lslboost/range/algorithm/reverse_copy.hpp>
#include <lslboost/range/algorithm/rotate.hpp>
#include <lslboost/range/algorithm/rotate_copy.hpp>
#include <lslboost/range/algorithm/sort.hpp>
#include <lslboost/range/algorithm/stable_partition.hpp>
#include <lslboost/range/algorithm/stable_sort.hpp>
#include <lslboost/range/algorithm/transform.hpp>
#include <lslboost/range/algorithm/unique.hpp>
#include <lslboost/range/algorithm/unique_copy.hpp>

// Binary search
#include <lslboost/range/algorithm/binary_search.hpp>
#include <lslboost/range/algorithm/equal_range.hpp>
#include <lslboost/range/algorithm/lower_bound.hpp>
#include <lslboost/range/algorithm/upper_bound.hpp>

// Set operations of sorted ranges
#include <lslboost/range/algorithm/set_algorithm.hpp>

// Heap operations
#include <lslboost/range/algorithm/heap_algorithm.hpp>

// Minimum and Maximum
#include <lslboost/range/algorithm/max_element.hpp>
#include <lslboost/range/algorithm/min_element.hpp>

// Permutations
#include <lslboost/range/algorithm/permutation.hpp>

#endif // include guard

