#include "compute/compute.h"
#include "core/must.h"
#include "eigen/Eigen"

void must_approx_equal(float x, float y, float tolerance = 0.01) {
  if (std::abs(x - y) > tolerance) FAIL(x, " vs ", y);
}

void Main() {
  compute::device device = compute::system::default_device();
  std::cerr << "Device: " << device.name() << std::endl;

  compute::context context(device);
  compute::program program = compute::program::create_with_source(R"(

        float get(global const float* p, ulong rows, ulong cols, ulong row, ulong col) {
          if (row < rows && col < cols)
            return p[row + col*rows];
          else
            return 0;
        }

        void set(global float* p, ulong rows, ulong cols, ulong row, ulong col, float val) {
          if (row < rows && col < cols)
            p[row + col*rows] = val;
        }

        #define cache_width 16
        #define cache_row (get_local_id(0))
        #define cache_col (get_local_id(1))
        #define global_row (get_global_id(0))
        #define global_col (get_global_id(1))

        kernel void matmul(const ulong rows_a, const ulong cols_a_rows_b, const ulong cols_b, global const float* a, global const float* b, global float* c) {
          local float cache_c[cache_width][cache_width];
          cache_c[cache_row][cache_col] = 0;
          barrier(CLK_LOCAL_MEM_FENCE);

          local float cache_a[cache_width][cache_width];
          local float cache_b[cache_width][cache_width];

          const ulong num_blocks = (cols_a_rows_b + cache_width - 1) / cache_width;
          for (ulong block = 0; block < num_blocks; ++block) {
            cache_a[cache_row][cache_col] = get(a, rows_a, cols_a_rows_b, global_row, block*cache_width+cache_col);
            cache_b[cache_row][cache_col] = get(b, cols_a_rows_b, cols_b, block*cache_width+cache_row, global_col);
            barrier(CLK_GLOBAL_MEM_FENCE);
            barrier(CLK_LOCAL_MEM_FENCE);

            private float total = 0;
            for (ulong pos = 0; pos < cache_width; ++pos) {
              total += cache_a[cache_row][pos] * cache_b[pos][cache_col];
            }
            cache_c[cache_row][cache_col] += total;
            barrier(CLK_LOCAL_MEM_FENCE);
          }
          set(c, rows_a, cols_b, global_row, global_col, cache_c[cache_row][cache_col]);
        }

        )",
                                                                  context);
  try {
    program.build();
  } catch (boost::compute::opencl_error& e) {
    std::cerr << program.build_log() << std::endl;
    throw;
  }
  compute::command_queue command_queue(context, device);

  compute::kernel matmul = program.create_kernel("matmul");

  constexpr size_t W = 8192;
  Eigen::MatrixXf a = Eigen::MatrixXf::Random(W, W);
  Eigen::MatrixXf b = Eigen::MatrixXf::Random(W, W);

  Eigen::MatrixXf c = a * b;

  compute::buffer buffer_a(context, sizeof(float) * W * W);
  compute::buffer buffer_b(context, sizeof(float) * W * W);
  compute::buffer buffer_c(context, sizeof(float) * W * W);

  command_queue.enqueue_write_buffer(buffer_a, 0 /*offset*/,
                                     sizeof(float) * W * W, a.data());
  command_queue.enqueue_write_buffer(buffer_b, 0 /*offset*/,
                                     sizeof(float) * W * W, b.data());
  command_queue.finish();
  matmul.set_arg(0, W);
  matmul.set_arg(1, W);
  matmul.set_arg(2, W);
  matmul.set_arg(3, buffer_a);
  matmul.set_arg(4, buffer_b);
  matmul.set_arg(5, buffer_c);

  command_queue.enqueue_nd_range_kernel(matmul, compute::extents<2>{0, 0},
                                        compute::extents<2>{W, W},
                                        compute::extents<2>{16, 16});
  command_queue.finish();

  std::vector<float> output(W * W);
  command_queue.enqueue_read_buffer(buffer_c, 0 /*offset*/,
                                    sizeof(float) * W * W, output.data());
  command_queue.finish();
  for (size_t row = 0; row < W; ++row)
    for (size_t col = 0; col < W; ++col)
      must_approx_equal(output[row + col * W], c(row, col));
}
