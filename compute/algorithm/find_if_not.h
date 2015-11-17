//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_FIND_IF_NOT_HPP
#define BOOST_COMPUTE_ALGORITHM_FIND_IF_NOT_HPP

#include "compute/system.h"
#include "compute/functional.h"
#include "compute/command_queue.h"
#include "compute/algorithm/find_if.h"

namespace boost {
namespace compute {

/// Returns an iterator pointing to the first element in the range
/// [\p first, \p last) for which \p predicate returns \c false.
///
/// \see find_if()
template <class InputIterator, class UnaryPredicate>
inline InputIterator find_if_not(
    InputIterator first, InputIterator last, UnaryPredicate predicate,
    command_queue &queue = system::default_queue()) {
  return ::boost::compute::find_if(first, last, not1(predicate), queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_FIND_IF_NOT_HPP
