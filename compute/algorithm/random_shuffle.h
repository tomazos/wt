//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_RANDOM_SHUFFLE_HPP
#define BOOST_COMPUTE_ALGORITHM_RANDOM_SHUFFLE_HPP

#include <vector>
#include <algorithm>

#include <boost/range/algorithm_ext/iota.hpp>

#include "compute/system.h"
#include "compute/functional.h"
#include "compute/command_queue.h"
#include "compute/container/vector.h"
#include "compute/algorithm/scatter.h"
#include "compute/detail/iterator_range_size.h"

namespace boost {
namespace compute {

/// Randomly shuffles the elements in the range [\p first, \p last).
///
/// \see scatter()
template <class Iterator>
inline void random_shuffle(Iterator first, Iterator last,
                           command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<Iterator>::value_type value_type;

  size_t count = detail::iterator_range_size(first, last);
  if (count == 0) {
    return;
  }

  // generate shuffled indices on the host
  std::vector<cl_uint> random_indices(count);
  boost::iota(random_indices, 0);
  std::random_shuffle(random_indices.begin(), random_indices.end());

  // copy random indices to the device
  const context &context = queue.get_context();
  vector<cl_uint> indices(count, context);
  ::boost::compute::copy(random_indices.begin(), random_indices.end(),
                         indices.begin(), queue);

  // make a copy of the values on the device
  vector<value_type> tmp(count, context);
  ::boost::compute::copy(first, last, tmp.begin(), queue);

  // write values to their new locations
  ::boost::compute::scatter(tmp.begin(), tmp.end(), indices.begin(), first,
                            queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_RANDOM_SHUFFLE_HPP
