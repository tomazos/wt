#include "compute/compute.h"

#include "core/must.h"
#include "gtest/gtest.h"

TEST(ComputeTest, Smoke) {
  for (const auto& p : compute::system::platforms()) {
    for (const auto& d : p.devices()) {
      LOGEXPR(p.name());
      LOGEXPR(d.name());
    }
  }
}

TEST(ComputeTest, Kernel) {
  constexpr size_t N = 1024 * 1024;
  for (const auto& platform : compute::system::platforms()) {
    for (const auto& device : platform.devices()) {
      compute::context context(device);
      compute::program program = compute::program::create_with_source(R"(

        void whee(global const float* i, global float* o) {
          *o = sin(*i);
        }

        kernel void hello(global const float* input, global float* output) {
          const size_t i = get_global_id(0);
          whee(input+i,output+i);
        }

        )",
                                                                      context);
      program.build();
      compute::command_queue command_queue(context, device);

      compute::kernel kernel = program.create_kernel("hello");
      compute::buffer input_buffer(context, sizeof(float) * N);
      compute::buffer output_buffer(context, sizeof(float) * N);

      std::vector<float> input(N);
      for (size_t i = 0; i < N; ++i) input[i] = i;

      command_queue.enqueue_write_buffer(input_buffer, 0 /*offset*/,
                                         sizeof(float) * N, input.data());

      kernel.set_arg(0, input_buffer);
      kernel.set_arg(1, output_buffer);

      command_queue.enqueue_nd_range_kernel(kernel, compute::extents<1>{0},
                                            compute::extents<1>{N},
                                            compute::extents<1>{1});

      std::vector<float> output(N);
      command_queue.enqueue_read_buffer(output_buffer, 0 /*offset*/,
                                        sizeof(float) * N, output.data());

      for (size_t i = 0; i < N; ++i) EXPECT_FLOAT_EQ(sin(input[i]), output[i]);
    }
  }
}
