//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_LOWER_BOUND_HPP
#define BOOST_COMPUTE_ALGORITHM_LOWER_BOUND_HPP

#include "compute/lambda.h"
#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/detail/binary_find.h"

namespace boost {
namespace compute {

/// Returns an iterator pointing to the first element in the sorted
/// range [\p first, \p last) that is not less than \p value.
///
/// \see upper_bound()
template <class InputIterator, class T>
inline InputIterator lower_bound(
    InputIterator first, InputIterator last, const T &value,
    command_queue &queue = system::default_queue()) {
  using ::boost::compute::_1;

  InputIterator position = detail::binary_find(first, last, _1 >= value, queue);

  return position;
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_LOWER_BOUND_HPP
