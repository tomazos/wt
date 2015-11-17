//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_COUNT_IF_HPP
#define BOOST_COMPUTE_ALGORITHM_COUNT_IF_HPP

#include "compute/device.h"
#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/detail/count_if_with_ballot.h"
#include "compute/algorithm/detail/count_if_with_reduce.h"
#include "compute/algorithm/detail/count_if_with_threads.h"
#include "compute/algorithm/detail/serial_count_if.h"
#include "compute/detail/iterator_range_size.h"

namespace boost {
namespace compute {

/// Returns the number of elements in the range [\p first, \p last)
/// for which \p predicate returns \c true.
template <class InputIterator, class Predicate>
inline size_t count_if(InputIterator first, InputIterator last,
                       Predicate predicate,
                       command_queue &queue = system::default_queue()) {
  const device &device = queue.get_device();

  size_t input_size = detail::iterator_range_size(first, last);
  if (input_size == 0) {
    return 0;
  }

  if (device.type() & device::cpu) {
    if (input_size < 1024) {
      return detail::serial_count_if(first, last, predicate, queue);
    } else {
      return detail::count_if_with_threads(first, last, predicate, queue);
    }
  } else {
    if (input_size < 32) {
      return detail::serial_count_if(first, last, predicate, queue);
    } else {
      return detail::count_if_with_reduce(first, last, predicate, queue);
    }
  }
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_COUNT_IF_HPP
