//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://boostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef BOOST_COMPUTE_KERNEL_HPP
#define BOOST_COMPUTE_KERNEL_HPP

#include <string>

#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>

#include "compute/config.h"
#include "compute/program.h"
#include "compute/exception.h"
#include "compute/type_traits/is_fundamental.h"
#include "compute/detail/get_object_info.h"
#include "compute/detail/assert_cl_success.h"
#include "compute/memory/svm_ptr.h"

namespace boost {
namespace compute {
namespace detail {

template <class T>
struct set_kernel_arg;

}  // end detail namespace

/// \class kernel
/// \brief A compute kernel.
///
/// \see command_queue, program
class kernel {
 public:
  /// Creates a null kernel object.
  kernel() : m_kernel(0) {}

  /// Creates a new kernel object for \p kernel. If \p retain is
  /// \c true, the reference count for \p kernel will be incremented.
  explicit kernel(cl_kernel kernel, bool retain = true) : m_kernel(kernel) {
    if (m_kernel && retain) {
      clRetainKernel(m_kernel);
    }
  }

  /// Creates a new kernel object with \p name from \p program.
  kernel(const program &program, const std::string &name) {
    cl_int error = 0;
    m_kernel = clCreateKernel(program.get(), name.c_str(), &error);

    if (!m_kernel) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
  }

  /// Creates a new kernel object as a copy of \p other.
  kernel(const kernel &other) : m_kernel(other.m_kernel) {
    if (m_kernel) {
      clRetainKernel(m_kernel);
    }
  }

  /// Copies the kernel object from \p other to \c *this.
  kernel &operator=(const kernel &other) {
    if (this != &other) {
      if (m_kernel) {
        clReleaseKernel(m_kernel);
      }

      m_kernel = other.m_kernel;

      if (m_kernel) {
        clRetainKernel(m_kernel);
      }
    }

    return *this;
  }

#ifndef BOOST_COMPUTE_NO_RVALUE_REFERENCES
  /// Move-constructs a new kernel object from \p other.
  kernel(kernel &&other) BOOST_NOEXCEPT : m_kernel(other.m_kernel) {
    other.m_kernel = 0;
  }

  /// Move-assigns the kernel from \p other to \c *this.
  kernel &operator=(kernel &&other) BOOST_NOEXCEPT {
    if (m_kernel) {
      clReleaseKernel(m_kernel);
    }

    m_kernel = other.m_kernel;
    other.m_kernel = 0;

    return *this;
  }
#endif  // BOOST_COMPUTE_NO_RVALUE_REFERENCES

  /// Destroys the kernel object.
  ~kernel() {
    if (m_kernel) {
      BOOST_COMPUTE_ASSERT_CL_SUCCESS(clReleaseKernel(m_kernel));
    }
  }

  /// Returns a reference to the underlying OpenCL kernel object.
  cl_kernel &get() const { return const_cast<cl_kernel &>(m_kernel); }

  /// Returns the function name for the kernel.
  std::string name() const {
    return get_info<std::string>(CL_KERNEL_FUNCTION_NAME);
  }

  /// Returns the number of arguments for the kernel.
  size_t arity() const { return get_info<cl_uint>(CL_KERNEL_NUM_ARGS); }

  /// Returns the program for the kernel.
  program get_program() const {
    return program(get_info<cl_program>(CL_KERNEL_PROGRAM));
  }

  /// Returns the context for the kernel.
  context get_context() const {
    return context(get_info<cl_context>(CL_KERNEL_CONTEXT));
  }

  /// Returns information about the kernel.
  ///
  /// \see_opencl_ref{clGetKernelInfo}
  template <class T>
  T get_info(cl_kernel_info info) const {
    return detail::get_object_info<T>(clGetKernelInfo, m_kernel, info);
  }

  /// \overload
  template <int Enum>
  typename detail::get_object_info_type<kernel, Enum>::type get_info() const;

#if defined(CL_VERSION_1_2) || defined(BOOST_COMPUTE_DOXYGEN_INVOKED)
  /// Returns information about the argument at \p index.
  ///
  /// For example, to get the name of the first argument:
  /// \code
  /// std::string arg = kernel.get_arg_info<std::string>(0, CL_KERNEL_ARG_NAME);
  /// \endcode
  ///
  /// Note, this function requires that the program be compiled with the
  /// \c "-cl-kernel-arg-info" flag. For example:
  /// \code
  /// program.build("-cl-kernel-arg-info");
  /// \endcode
  ///
  /// \opencl_version_warning{1,2}
  ///
  /// \see_opencl_ref{clGetKernelArgInfo}
  template <class T>
  T get_arg_info(size_t index, cl_kernel_arg_info info) const {
    return detail::get_object_info<T>(clGetKernelArgInfo, m_kernel, info,
                                      index);
  }
#endif  // CL_VERSION_1_2

  /// Returns work-group information for the kernel with \p device.
  ///
  /// \see_opencl_ref{clGetKernelWorkGroupInfo}
  template <class T>
  T get_work_group_info(const device &device,
                        cl_kernel_work_group_info info) const {
    return detail::get_object_info<T>(clGetKernelWorkGroupInfo, m_kernel, info,
                                      device.id());
  }

  /// Sets the argument at \p index to \p value with \p size.
  ///
  /// \see_opencl_ref{clSetKernelArg}
  void set_arg(size_t index, size_t size, const void *value) {
    BOOST_ASSERT(index < arity());

    cl_int ret =
        clSetKernelArg(m_kernel, static_cast<cl_uint>(index), size, value);
    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }
  }

  /// Sets the argument at \p index to \p value.
  ///
  /// For built-in types (e.g. \c float, \c int4_), this is equivalent to
  /// calling set_arg(index, sizeof(type), &value).
  ///
  /// Additionally, this method is specialized for device memory objects
  /// such as buffer and image2d. This allows for them to be passed directly
  /// without having to extract their underlying cl_mem object.
  ///
  /// This method is also specialized for device container types such as
  /// vector<T> and array<T, N>. This allows for them to be passed directly
  /// as kernel arguments without having to extract their underlying buffer.
  ///
  /// For setting local memory arguments (e.g. "__local float *buf"), the
  /// local_buffer<T> class may be used:
  /// \code
  /// // set argument to a local buffer with storage for 32 float's
  /// kernel.set_arg(0, local_buffer<float>(32));
  /// \endcode
  template <class T>
  void set_arg(size_t index, const T &value) {
    // if you get a compilation error pointing here it means you
    // attempted to set a kernel argument from an invalid type.
    detail::set_kernel_arg<T>()(*this, index, value);
  }

  /// \internal_
  void set_arg(size_t index, const cl_mem mem) {
    set_arg(index, sizeof(cl_mem), static_cast<const void *>(&mem));
  }

  /// \internal_
  void set_arg(size_t index, const cl_sampler sampler) {
    set_arg(index, sizeof(cl_sampler), static_cast<const void *>(&sampler));
  }

  /// \internal_
  template <class T>
  void set_arg(size_t index, const svm_ptr<T> ptr) {
#ifdef CL_VERSION_2_0
    cl_int ret = clSetKernelArgSVMPointer(m_kernel, index, ptr.get());
    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }
#else
    BOOST_THROW_EXCEPTION(opencl_error(CL_INVALID_ARG_VALUE));
#endif
  }

#ifndef BOOST_COMPUTE_NO_VARIADIC_TEMPLATES
  /// Sets the arguments for the kernel to \p args.
  template <class... T>
  void set_args(T &&... args) {
    BOOST_ASSERT(sizeof...(T) <= arity());

    _set_args<0>(args...);
  }
#endif  // BOOST_COMPUTE_NO_VARIADIC_TEMPLATES

#if defined(CL_VERSION_2_0) || defined(BOOST_COMPUTE_DOXYGEN_INVOKED)
  /// Sets additional execution information for the kernel.
  ///
  /// \opencl_version_warning{2,0}
  ///
  /// \see_opencl2_ref{clSetKernelExecInfo}
  void set_exec_info(cl_kernel_exec_info info, size_t size, const void *value) {
    cl_int ret = clSetKernelExecInfo(m_kernel, info, size, value);
    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }
  }
#endif  // CL_VERSION_2_0

  /// Returns \c true if the kernel is the same at \p other.
  bool operator==(const kernel &other) const {
    return m_kernel == other.m_kernel;
  }

  /// Returns \c true if the kernel is different from \p other.
  bool operator!=(const kernel &other) const {
    return m_kernel != other.m_kernel;
  }

  /// \internal_
  operator cl_kernel() const { return m_kernel; }

  /// \internal_
  static kernel create_with_source(const std::string &source,
                                   const std::string &name,
                                   const context &context) {
    return program::build_with_source(source, context).create_kernel(name);
  }

 private:
#ifndef BOOST_NO_VARIADIC_TEMPLATES
  /// \internal_
  template <size_t N>
  void _set_args() {}

  /// \internal_
  template <size_t N, class T, class... Args>
  void _set_args(T &&arg, Args &&... rest) {
    set_arg(N, arg);
    _set_args<N + 1>(rest...);
  }
#endif  // BOOST_NO_VARIADIC_TEMPLATES

 private:
  cl_kernel m_kernel;
};

inline kernel program::create_kernel(const std::string &name) const {
  return kernel(*this, name);
}

/// \internal_ define get_info() specializations for kernel
BOOST_COMPUTE_DETAIL_DEFINE_GET_INFO_SPECIALIZATIONS(
    kernel,
    ((std::string, CL_KERNEL_FUNCTION_NAME))((cl_uint, CL_KERNEL_NUM_ARGS))(
        (cl_uint, CL_KERNEL_REFERENCE_COUNT))((cl_context, CL_KERNEL_CONTEXT))(
        (cl_program, CL_KERNEL_PROGRAM)))

#ifdef CL_VERSION_1_2
BOOST_COMPUTE_DETAIL_DEFINE_GET_INFO_SPECIALIZATIONS(kernel,
                                                     ((std::string,
                                                       CL_KERNEL_ATTRIBUTES)))
#endif  // CL_VERSION_1_2

namespace detail {

// set_kernel_arg implementation for built-in types
template <class T>
struct set_kernel_arg {
  typename boost::enable_if<is_fundamental<T> >::type operator()(
      kernel &kernel_, size_t index, const T &value) {
    kernel_.set_arg(index, sizeof(T), &value);
  }
};

// set_kernel_arg specialization for char (different from built-in cl_char)
template <>
struct set_kernel_arg<char> {
  void operator()(kernel &kernel_, size_t index, const char c) {
    kernel_.set_arg(index, sizeof(char), &c);
  }
};

}  // end detail namespace
}  // end namespace compute
}  // end namespace boost

#endif  // BOOST_COMPUTE_KERNEL_HPP
