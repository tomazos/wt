//---------------------------------------------------------------------------//
// Copyright (c) 2013-2014 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_ADJACENT_DIFFERENCE_HPP
#define BOOST_COMPUTE_ALGORITHM_ADJACENT_DIFFERENCE_HPP

#include <iterator>

#include "compute/system.h"
#include "compute/command_queue.h"
#include "compute/detail/meta_kernel.h"
#include "compute/detail/iterator_range_size.h"
#include "compute/functional/operator.h"
#include "compute/container/vector.h"

namespace boost {
namespace compute {

/// Stores the difference of each pair of consecutive values in the range
/// [\p first, \p last) to the range beginning at \p result. If \p op is not
/// provided, \c minus<T> is used.
///
/// \param first first element in the input range
/// \param last last element in the input range
/// \param result first element in the output range
/// \param op binary difference function
/// \param queue command queue to perform the operation
///
/// \return \c OutputIterator to the end of the result range
///
/// \see adjacent_find()
template <class InputIterator, class OutputIterator, class BinaryFunction>
inline OutputIterator adjacent_difference(
    InputIterator first, InputIterator last, OutputIterator result,
    BinaryFunction op, command_queue &queue = system::default_queue()) {
  if (first == last) {
    return result;
  }

  size_t count = detail::iterator_range_size(first, last);

  detail::meta_kernel k("adjacent_difference");

  k << "const uint i = get_global_id(0);\n"
    << "if(i == 0){\n"
    << "    " << result[k.var<uint_>("0")] << " = " << first[k.var<uint_>("0")]
    << ";\n"
    << "}\n"
    << "else {\n"
    << "    " << result[k.var<uint_>("i")] << " = "
    << op(first[k.var<uint_>("i")], first[k.var<uint_>("i-1")]) << ";\n"
    << "}\n";

  k.exec_1d(queue, 0, count, 1);

  return result + count;
}

/// \overload
template <class InputIterator, class OutputIterator>
inline OutputIterator adjacent_difference(
    InputIterator first, InputIterator last, OutputIterator result,
    command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator>::value_type value_type;

  if (first == result) {
    vector<value_type> temp(detail::iterator_range_size(first, last),
                            queue.get_context());
    copy(first, last, temp.begin(), queue);

    return ::boost::compute::adjacent_difference(
        temp.begin(), temp.end(), result, ::boost::compute::minus<value_type>(),
        queue);
  } else {
    return ::boost::compute::adjacent_difference(
        first, last, result, ::boost::compute::minus<value_type>(), queue);
  }
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_ADJACENT_DIFFERENCE_HPP
