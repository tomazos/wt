#pragma once

namespace boost {
namespace compute {}  // namespace compute
}  // namespace boost

namespace compute = boost::compute;

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#include "compute/algorithm.h"
#include "compute/allocator.h"
#include "compute/async.h"
#include "compute/buffer.h"
#include "compute/cl.h"
#include "compute/command_queue.h"
#include "compute/config.h"
#include "compute/container.h"
#include "compute/context.h"
#include "compute/device.h"
#include "compute/functional.h"
#include "compute/image.h"
#include "compute/iterator.h"
#include "compute/kernel.h"
#include "compute/lambda.h"
#include "compute/pipe.h"
#include "compute/platform.h"
#include "compute/program.h"
#include "compute/random.h"
#include "compute/svm.h"
#include "compute/system.h"
#include "compute/types.h"
#include "compute/user_event.h"
#include "compute/utility.h"
#include "compute/version.h"

#ifdef BOOST_COMPUTE_HAVE_HDR_CL_EXT
#include "compute/cl_ext.h"
#endif
