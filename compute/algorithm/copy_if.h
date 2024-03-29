//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_COPY_IF_HPP
#define BOOST_COMPUTE_ALGORITHM_COPY_IF_HPP

#include "compute/algorithm/transform_if.h"
#include "compute/functional/identity.h"

namespace boost {
namespace compute {
namespace detail {

// like the copy_if() algorithm but writes the indices of the values for which
// predicate returns true.
template <class InputIterator, class OutputIterator, class Predicate>
inline OutputIterator copy_index_if(
    InputIterator first, InputIterator last, OutputIterator result,
    Predicate predicate, command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator>::value_type T;

  return detail::transform_if_impl(first, last, result, identity<T>(),
                                   predicate, true, queue);
}

}  // end detail namespace

/// Copies each element in the range [\p first, \p last) for which
/// \p predicate returns \c true to the range beginning at \p result.
template <class InputIterator, class OutputIterator, class Predicate>
inline OutputIterator copy_if(InputIterator first, InputIterator last,
                              OutputIterator result, Predicate predicate,
                              command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator>::value_type T;

  return ::boost::compute::transform_if(first, last, result, identity<T>(),
                                        predicate, queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_COPY_IF_HPP
