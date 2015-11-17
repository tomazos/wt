//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_NTH_ELEMENT_HPP
#define BOOST_COMPUTE_ALGORITHM_NTH_ELEMENT_HPP

#include "compute/command_queue.h"
#include "compute/algorithm/fill_n.h"
#include "compute/algorithm/find.h"
#include "compute/algorithm/partition.h"
#include "compute/algorithm/sort.h"
#include "compute/functional/bind.h"

namespace boost {
namespace compute {

/// Rearranges the elements in the range [\p first, \p last) such that
/// the \p nth element would be in that position in a sorted sequence.
template <class Iterator, class Compare>
inline void nth_element(Iterator first, Iterator nth, Iterator last,
                        Compare compare,
                        command_queue &queue = system::default_queue()) {
  if (nth == last) return;

  typedef typename std::iterator_traits<Iterator>::value_type value_type;

  while (1) {
    value_type value = nth.read(queue);

    using boost::compute::placeholders::_1;
    Iterator new_nth = partition(
        first, last, ::boost::compute::bind(compare, _1, value), queue);

    Iterator old_nth = find(new_nth, last, value, queue);

    value_type new_value = new_nth.read(queue);

    fill_n(new_nth, 1, value, queue);
    fill_n(old_nth, 1, new_value, queue);

    new_value = nth.read(queue);

    if (value == new_value) break;

    if (std::distance(first, nth) < std::distance(first, new_nth)) {
      last = new_nth;
    } else {
      first = new_nth;
    }
  }
}

/// \overload
template <class Iterator>
inline void nth_element(Iterator first, Iterator nth, Iterator last,
                        command_queue &queue = system::default_queue()) {
  if (nth == last) return;

  typedef typename std::iterator_traits<Iterator>::value_type value_type;

  less<value_type> less_than;

  return nth_element(first, nth, last, less_than, queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_NTH_ELEMENT_HPP
