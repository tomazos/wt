//---------------------------------------------------------------------------//
// Copyright (c) 2014 Roshan <thisisroshansmail@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_ALGORITHM_SET_SYMMETRIC_DIFFERENCE_HPP
#define BOOST_COMPUTE_ALGORITHM_SET_SYMMETRIC_DIFFERENCE_HPP

#include <iterator>

#include "compute/algorithm/detail/compact.h"
#include "compute/algorithm/detail/balanced_path.h"
#include "compute/algorithm/exclusive_scan.h"
#include "compute/algorithm/fill_n.h"
#include "compute/container/vector.h"
#include "compute/detail/iterator_range_size.h"
#include "compute/detail/meta_kernel.h"
#include "compute/system.h"

namespace boost {
namespace compute {
namespace detail {

///
/// \brief Serial set symmetric difference kernel class
///
/// Subclass of meta_kernel to perform serial set symmetric
/// difference after tiling
///
class serial_set_symmetric_difference_kernel : meta_kernel {
 public:
  unsigned int tile_size;

  serial_set_symmetric_difference_kernel()
      : meta_kernel("set_symmetric_difference") {
    tile_size = 4;
  }

  template <class InputIterator1, class InputIterator2, class InputIterator3,
            class InputIterator4, class OutputIterator1, class OutputIterator2>
  void set_range(InputIterator1 first1, InputIterator2 first2,
                 InputIterator3 tile_first1, InputIterator3 tile_last1,
                 InputIterator4 tile_first2, OutputIterator1 result,
                 OutputIterator2 counts) {
    m_count = iterator_range_size(tile_first1, tile_last1) - 1;

    *this << "uint i = get_global_id(0);\n"
          << "uint start1 = " << tile_first1[expr<uint_>("i")] << ";\n"
          << "uint end1 = " << tile_first1[expr<uint_>("i+1")] << ";\n"
          << "uint start2 = " << tile_first2[expr<uint_>("i")] << ";\n"
          << "uint end2 = " << tile_first2[expr<uint_>("i+1")] << ";\n"
          << "uint index = i*" << tile_size << ";\n"
          << "uint count = 0;\n"
          << "while(start1<end1 && start2<end2)\n"
          << "{\n"
          << "   if(" << first1[expr<uint_>("start1")]
          << " == " << first2[expr<uint_>("start2")] << ")\n"
          << "   {\n"
          << "       start1++; start2++;\n"
          << "   }\n"
          << "   else if(" << first1[expr<uint_>("start1")] << " < "
          << first2[expr<uint_>("start2")] << ")\n"
          << "   {\n" << result[expr<uint_>("index")] << " = "
          << first1[expr<uint_>("start1")] << ";\n"
          << "       index++; count++;\n"
          << "       start1++;\n"
          << "   }\n"
          << "   else\n"
          << "   {\n" << result[expr<uint_>("index")] << " = "
          << first2[expr<uint_>("start2")] << ";\n"
          << "       index++; count++;\n"
          << "       start2++;\n"
          << "   }\n"
          << "}\n"
          << "while(start1<end1)\n"
          << "{\n" << result[expr<uint_>("index")] << " = "
          << first1[expr<uint_>("start1")] << ";\n"
          << "   index++; count++;\n"
          << "   start1++;\n"
          << "}\n"
          << "while(start2<end2)\n"
          << "{\n" << result[expr<uint_>("index")] << " = "
          << first2[expr<uint_>("start2")] << ";\n"
          << "   index++; count++;\n"
          << "   start2++;\n"
          << "}\n" << counts[expr<uint_>("i")] << " = count;\n";
  }

  event exec(command_queue &queue) {
    if (m_count == 0) {
      return event();
    }

    return exec_1d(queue, 0, m_count);
  }

 private:
  size_t m_count;
};

}  // end detail namespace

///
/// \brief Set symmetric difference algorithm
///
/// Finds the symmetric difference of the sorted range [first2, last2) from
/// the sorted range [first1, last1) and stores it in range starting at result
/// \return Iterator pointing to end of symmetric difference
///
/// \param first1 Iterator pointing to start of first set
/// \param last1 Iterator pointing to end of first set
/// \param first2 Iterator pointing to start of second set
/// \param last2 Iterator pointing to end of second set
/// \param result Iterator pointing to start of range in which the symmetric
/// difference will be stored
/// \param queue Queue on which to execute
///
template <class InputIterator1, class InputIterator2, class OutputIterator>
inline OutputIterator set_symmetric_difference(
    InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
    InputIterator2 last2, OutputIterator result,
    command_queue &queue = system::default_queue()) {
  typedef typename std::iterator_traits<InputIterator1>::value_type value_type;

  int tile_size = 1024;

  int count1 = detail::iterator_range_size(first1, last1);
  int count2 = detail::iterator_range_size(first2, last2);

  vector<uint_> tile_a((count1 + count2 + tile_size - 1) / tile_size + 1,
                       queue.get_context());
  vector<uint_> tile_b((count1 + count2 + tile_size - 1) / tile_size + 1,
                       queue.get_context());

  // Tile the sets
  detail::balanced_path_kernel tiling_kernel;
  tiling_kernel.tile_size = tile_size;
  tiling_kernel.set_range(first1, last1, first2, last2, tile_a.begin() + 1,
                          tile_b.begin() + 1);
  fill_n(tile_a.begin(), 1, 0, queue);
  fill_n(tile_b.begin(), 1, 0, queue);
  tiling_kernel.exec(queue);

  fill_n(tile_a.end() - 1, 1, count1, queue);
  fill_n(tile_b.end() - 1, 1, count2, queue);

  vector<value_type> temp_result(count1 + count2, queue.get_context());
  vector<uint_> counts((count1 + count2 + tile_size - 1) / tile_size + 1,
                       queue.get_context());
  fill_n(counts.end() - 1, 1, 0, queue);

  // Find individual symmetric differences
  detail::serial_set_symmetric_difference_kernel symmetric_difference_kernel;
  symmetric_difference_kernel.tile_size = tile_size;
  symmetric_difference_kernel.set_range(first1, first2, tile_a.begin(),
                                        tile_a.end(), tile_b.begin(),
                                        temp_result.begin(), counts.begin());

  symmetric_difference_kernel.exec(queue);

  exclusive_scan(counts.begin(), counts.end(), counts.begin(), queue);

  // Compact the results
  detail::compact_kernel compact_kernel;
  compact_kernel.tile_size = tile_size;
  compact_kernel.set_range(temp_result.begin(), counts.begin(), counts.end(),
                           result);

  compact_kernel.exec(queue);

  return result + (counts.end() - 1).read(queue);
}

}  // end compute namespace
}  // end boost namespace

#endif  // BOOST_COMPUTE_ALGORITHM_SET_SYMMETRIC_DIFFERENCE_HPP
