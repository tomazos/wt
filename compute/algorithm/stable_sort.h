//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_STABLE_SORT_HPP
#define BOOST_COMPUTE_ALGORITHM_STABLE_SORT_HPP

#include <iterator>

#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/detail/merge_sort_on_cpu.h"
#include "compute/algorithm/detail/radix_sort.h"
#include "compute/algorithm/detail/insertion_sort.h"
#include "compute/algorithm/reverse.h"
#include "compute/functional/operator.h"

namespace boost {
namespace compute {
namespace detail {

template <class Iterator, class Compare>
inline void dispatch_gpu_stable_sort(Iterator first, Iterator last,
                                     Compare compare, command_queue &queue) {
  ::boost::compute::detail::serial_insertion_sort(first, last, compare, queue);
}

template <class T>
inline typename boost::enable_if_c<is_radix_sortable<T>::value>::type
dispatch_gpu_stable_sort(buffer_iterator<T> first, buffer_iterator<T> last,
                         less<T>, command_queue &queue) {
  ::boost::compute::detail::radix_sort(first, last, queue);
}

template <class T>
inline typename boost::enable_if_c<is_radix_sortable<T>::value>::type
dispatch_gpu_stable_sort(buffer_iterator<T> first, buffer_iterator<T> last,
                         greater<T>, command_queue &queue) {
  // radix sort in ascending order
  ::boost::compute::detail::radix_sort(first, last, queue);

  // reverse range to descending order
  ::boost::compute::reverse(first, last, queue);
}

}  // end detail namespace

/// Sorts the values in the range [\p first, \p last) according to
/// \p compare. The relative order of identical values is preserved.
///
/// \see sort(), is_sorted()
template <class Iterator, class Compare>
inline void stable_sort(Iterator first, Iterator last, Compare compare,
                        command_queue &queue = system::default_queue()) {
  if (queue.get_device().type() & device::gpu) {
    ::boost::compute::detail::dispatch_gpu_stable_sort(first, last, compare,
                                                       queue);
  }
  ::boost::compute::detail::merge_sort_on_cpu(first, last, compare, queue);
}

/// \overload
template <class Iterator>
inline void stable_sort(Iterator first, Iterator last,
                        command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<Iterator>::value_type value_type;

  ::boost::compute::less<value_type> less;

  ::boost::compute::stable_sort(first, last, less, queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_STABLE_SORT_HPP
