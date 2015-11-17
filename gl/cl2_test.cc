#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_EXCEPTIONS 1
#define CL_HPP_CL_1_2_DEFAULT_BUILD 1

#include "gl/cl2.h"

#include "core/must.h"
#include "gtest/gtest.h"

namespace cl {

TEST(CL2Test, Smoke) {
  constexpr size_t N = 1024 * 1024;
  std::vector<Platform> platforms;
  Platform::get(&platforms);
  for (const Platform& platform : platforms) {
    LOGEXPR(platform.getInfo<CL_PLATFORM_NAME>());
    LOGEXPR(platform.getInfo<CL_PLATFORM_VERSION>());
    std::vector<Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    for (const Device& device : devices) {
      LOGEXPR(device.getInfo<CL_DEVICE_NAME>());
      LOGEXPR(device.getInfo<CL_DEVICE_VENDOR>());
      LOGEXPR(device.getInfo<CL_DEVICE_VERSION>());
      LOGEXPR(device.getInfo<CL_DEVICE_EXTENSIONS>());

      Context context(device);
      try {
        Program program(context, R"(

        void whee(global const float* i, global float* o) {
          *o = sin(*i);
        }

        kernel void hello(global const float* input, global float* output) {
          const size_t i = get_global_id(0);
          whee(input+i,output+i);
        }

        )",
                        true /*build*/);
        CommandQueue command_queue(context);
        //        std::vector<Kernel> kernels;
        //        program.createKernels(&kernels);
        //        EXPECT_EQ(kernels.size(), 1u);
        //        Kernel kernel = kernels.at(0);
        KernelFunctor kernel_functor(program, "hello");
        std::vector<float> input(N);
        for (size_t i = 0; i < N; ++i) input[i] = i;
        Buffer input_buffer(context, CL_MEM_READ_WRITE, sizeof(float) * N);
        Buffer output_buffer(context, CL_MEM_READ_WRITE, sizeof(float) * N);
        command_queue.enqueueWriteBuffer(input_buffer, true /*blocking*/,
                                         0 /*offset*/, sizeof(float) * N,
                                         input.data());
        kernel_functor(EnqueueArgs(command_queue, NDRange(N)), input_buffer,
                       output_buffer);
        std::vector<float> output(N);
        command_queue.enqueueReadBuffer(output_buffer, true /*blocking*/, 0,
                                        sizeof(float) * N, output.data());
        for (size_t i = 0; i < N; ++i)
          EXPECT_FLOAT_EQ(sin(input[i]), output[i]);

      } catch (const BuildError& build_error) {
        for (const auto& build_log : build_error.getBuildLog()) {
          std::cerr << build_log.second << std::endl;
        }
        throw;
      } catch (const Error& error) {
        std::cerr << error.err() << std::endl;
        throw;
      }
    }
  }
}

}  // namespace cl
