//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_COPY_N_HPP
#define BOOST_COMPUTE_ALGORITHM_COPY_N_HPP

#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/copy.h"

namespace boost {
namespace compute {

/// Copies \p count elements from \p first to \p result.
///
/// For example, to copy four values from the host to the device:
/// \code
/// // values on the host and vector on the device
/// float values[4] = { 1.f, 2.f, 3.f, 4.f };
/// boost::compute::vector<float> vec(4, context);
///
/// // copy from the host to the device
/// boost::compute::copy_n(values, 4, vec.begin(), queue);
/// \endcode
///
/// \see copy()
template <class InputIterator, class Size, class OutputIterator>
inline OutputIterator copy_n(InputIterator first, Size count,
                             OutputIterator result,
                             command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator>::difference_type
      difference_type;

  return ::boost::compute::copy(
      first, first + static_cast<difference_type>(count), result, queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_COPY_N_HPP
