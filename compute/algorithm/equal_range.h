//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_EQUAL_RANGE_HPP
#define BOOST_COMPUTE_ALGORITHM_EQUAL_RANGE_HPP

#include <utility>

#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/lower_bound.h"
#include "compute/algorithm/upper_bound.h"

namespace boost {
namespace compute {

/// Returns a pair of iterators containing the range of values equal
/// to \p value in the sorted range [\p first, \p last).
template <class InputIterator, class T>
inline std::pair<InputIterator, InputIterator> equal_range(
    InputIterator first, InputIterator last, const T &value,
    command_queue &queue = system::default_queue()) {
  return std::make_pair(
      ::boost::compute::lower_bound(first, last, value, queue),
      ::boost::compute::upper_bound(first, last, value, queue));
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_EQUAL_RANGE_HPP
