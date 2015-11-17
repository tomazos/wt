//---------------------------------------------------------------------------//
// Copyright (c) 2014 Roshan <thisisroshansmail@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_UNIQUE_HPP
#define BOOST_COMPUTE_ALGORITHM_UNIQUE_HPP

#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/unique_copy.h"
#include "compute/container/vector.h"
#include "compute/functional/operator.h"

namespace boost {
namespace compute {

/// Removes all consecutive duplicate elements (determined by \p op) from the
/// range [first, last). If \p op is not provided, the equality operator is
/// used.
///
/// \param first first element in the input range
/// \param last last element in the input range
/// \param op binary operator used to check for uniqueness
/// \param queue command queue to perform the operation
///
/// \return \c InputIterator to the new logical end of the range
///
/// \see unique_copy()
template <class InputIterator, class BinaryPredicate>
inline InputIterator unique(InputIterator first, InputIterator last,
                            BinaryPredicate op,
                            command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator>::value_type value_type;

  vector<value_type> temp(first, last, queue);

  return ::boost::compute::unique_copy(temp.begin(), temp.end(), first, op,
                                       queue);
}

/// \overload
template <class InputIterator>
inline InputIterator unique(InputIterator first, InputIterator last,
                            command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator>::value_type value_type;

  return ::boost::compute::unique(
      first, last, ::boost::compute::equal_to<value_type>(), queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_UNIQUE_HPP
