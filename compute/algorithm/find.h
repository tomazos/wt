//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_FIND_HPP
#define BOOST_COMPUTE_ALGORITHM_FIND_HPP

#include "compute/lambda.h"
#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/find_if.h"
#include "compute/type_traits/vector_size.h"

namespace boost {
namespace compute {

/// Returns an iterator pointing to the first element in the range
/// [\p first, \p last) that equals \p value.
template <class InputIterator, class T>
inline InputIterator find(InputIterator first, InputIterator last,
                          const T &value,
                          command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator>::value_type value_type;

  using ::boost::compute::_1;
  using ::boost::compute::lambda::all;

  if (vector_size<value_type>::value == 1) {
    return ::boost::compute::find_if(first, last, _1 == value, queue);
  } else {
    return ::boost::compute::find_if(first, last, all(_1 == value), queue);
  }
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_FIND_HPP
