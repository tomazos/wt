//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_SORT_BY_KEY_HPP
#define BOOST_COMPUTE_ALGORITHM_SORT_BY_KEY_HPP

#include <iterator>

#include <boost/utility/enable_if.hpp>

#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/algorithm/detail/merge_sort_on_cpu.h"
#include "compute/algorithm/detail/insertion_sort.h"
#include "compute/algorithm/detail/radix_sort.h"
#include "compute/algorithm/reverse.h"
#include "compute/detail/iterator_range_size.h"

namespace boost {
namespace compute {

namespace detail {

template <class KeyIterator, class ValueIterator>
inline void dispatch_gpu_sort_by_key(
    KeyIterator keys_first, KeyIterator keys_last, ValueIterator values_first,
    less<typename std::iterator_traits<KeyIterator>::value_type> compare,
    command_queue &queue,
    typename boost::enable_if_c<is_radix_sortable<typename std::iterator_traits<
        KeyIterator>::value_type>::value>::type * = 0) {
  size_t count = detail::iterator_range_size(keys_first, keys_last);

  if (count < 32) {
    detail::serial_insertion_sort_by_key(keys_first, keys_last, values_first,
                                         compare, queue);
  } else {
    detail::radix_sort_by_key(keys_first, keys_last, values_first, queue);
  }
}

template <class KeyIterator, class ValueIterator>
inline void dispatch_gpu_sort_by_key(
    KeyIterator keys_first, KeyIterator keys_last, ValueIterator values_first,
    greater<typename std::iterator_traits<KeyIterator>::value_type> compare,
    command_queue &queue,
    typename boost::enable_if_c<is_radix_sortable<typename std::iterator_traits<
        KeyIterator>::value_type>::value>::type * = 0) {
  size_t count = detail::iterator_range_size(keys_first, keys_last);

  if (count < 32) {
    detail::serial_insertion_sort_by_key(keys_first, keys_last, values_first,
                                         compare, queue);
  } else {
    // radix sorts in ascending order
    detail::radix_sort_by_key(keys_first, keys_last, values_first, queue);

    // Reverse keys, values for descending order
    ::boost::compute::reverse(keys_first, keys_last, queue);
    ::boost::compute::reverse(values_first, values_first + count, queue);
  }
}

template <class KeyIterator, class ValueIterator, class Compare>
inline void dispatch_gpu_sort_by_key(KeyIterator keys_first,
                                     KeyIterator keys_last,
                                     ValueIterator values_first,
                                     Compare compare, command_queue &queue) {
  detail::serial_insertion_sort_by_key(keys_first, keys_last, values_first,
                                       compare, queue);
}

template <class KeyIterator, class ValueIterator, class Compare>
inline void dispatch_sort_by_key(KeyIterator keys_first, KeyIterator keys_last,
                                 ValueIterator values_first, Compare compare,
                                 command_queue &queue) {
  if (queue.get_device().type() & device::gpu) {
    dispatch_gpu_sort_by_key(keys_first, keys_last, values_first, compare,
                             queue);
    return;
  }
  ::boost::compute::detail::merge_sort_by_key_on_cpu(
      keys_first, keys_last, values_first, compare, queue);
}
}

/// Performs a key-value sort using the keys in the range [\p keys_first,
/// \p keys_last) on the values in the range [\p values_first,
/// \p values_first \c + (\p keys_last \c - \p keys_first)) using \p compare.
///
/// If no compare function is specified, \c less is used.
///
/// \see sort()

template <class KeyIterator, class ValueIterator, class Compare>
inline void sort_by_key(KeyIterator keys_first, KeyIterator keys_last,
                        ValueIterator values_first, Compare compare,
                        command_queue &queue = system::default_queue()) {
  ::boost::compute::detail::dispatch_sort_by_key(keys_first, keys_last,
                                                 values_first, compare, queue);
}

/// \overload
template <class KeyIterator, class ValueIterator>
inline void sort_by_key(KeyIterator keys_first, KeyIterator keys_last,
                        ValueIterator values_first,
                        command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<KeyIterator>::value_type key_type;

  ::boost::compute::sort_by_key(keys_first, keys_last, values_first,
                                less<key_type>(), queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_SORT_BY_KEY_HPP
