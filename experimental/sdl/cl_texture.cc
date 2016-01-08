#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

#include <SDL2/SDL.h>

#include <cstring>
#include <map>
#include <set>
#include <sys/time.h>

#include "compute/compute.h"
#include "compute/interop/opengl.h"
#include "core/must.h"
#include "core/random.h"
#include "gl/attrib_arrays.h"
#include "gl/geometry.h"
#include "gl/program.h"
#include "gl/shader.h"
#include "gl/sampler.h"
#include "gl/system.h"
#include "gl/texture.h"
#include "sdl/pixel_format.h"
#include "sdl/pov.h"
#include "sdl/surface.h"

std::pair<compute::platform, compute::device> GetGPU() {
  for (auto platform : compute::system::platforms()) {
    for (auto device : platform.devices(CL_DEVICE_TYPE_GPU)) {
      return {platform, device};
    }
  }
  FAIL();
}

using compute::dim;

constexpr auto vertex_shader_code = R"(

  #version 430 core

  #define M_PI 3.1415926535897932384626433832795

  in vec4 position;
  uniform mat4 transform;
  out vec2 texpos;

  void main(void)
  {
    gl_Position = transform * position;
    vec2 planepos = position.xy;
    planepos -= 2*floor((planepos+1)/2);
    texpos = vec2((1+position.x)/2,(1-position.y)/2);
  }

)";

constexpr auto fragment_shader_code = R"(

  #version 430 core

  in vec2 texpos;
  out vec4 color_out;
  uniform sampler2D cat_texture;
  void main(void)
  {
    color_out = texture2D(cat_texture, texpos);
    color_out = vec4(color_out.b, color_out.g, color_out.r, 1);
  }

)";

constexpr auto board_kernel_code = R"(

  #define kRows 2048
  #define kBitWidth kRows
  #define kByteWidth (kBitWidth / 8)
  #define kByteArea (kRows * kByteWidth)
  #define kCacheInner 8
  #define kCacheOuter (kCacheInner + 2)
  #define kGroupCols (kByteWidth / kCacheInner)
  #define kGroupRows (kRows / kCacheInner)

  bool get_cell(global uchar* board, int2 coord) {
    if (coord.x < 0 || coord.x >= kBitWidth || coord.y < 0 || coord.y >= kRows)
      return false;
    int pos = coord.x + coord.y*kBitWidth;
    int byte_pos = pos / 8;
    int bit_pos = (1 << (pos %8));
    uchar byte_val = board[byte_pos];
    if ((byte_val & bit_pos) != 0)
      return true;
    else
      return false;
  }

  uchar get_board_byte(global uchar* board, int x, int y) {
    if (x == -1)
      x = kByteWidth - 1;
    else if (x == kByteWidth)
      x = 0;

    if (y == -1)
      y = kRows - 1;
    else if (y == kRows)
      y = 0;

    return board[x + kByteWidth*y];
  }

  kernel void update_board(global uchar* old_board, global uchar* new_board) {
    const int outer_cache_x = get_local_id(0);
    const int outer_cache_y = get_local_id(1);
    const int inner_cache_x = outer_cache_x-1;
    const int inner_cache_y = outer_cache_y-1;

    int anchor_x = get_group_id(0)*kCacheInner;
    int anchor_y = get_group_id(1)*kCacheInner;

    int global_x = anchor_x + inner_cache_x;
    int global_y = anchor_y + inner_cache_y;

    local uchar input_cache[kCacheOuter][kCacheOuter];

    input_cache[outer_cache_x][outer_cache_y] = get_board_byte(old_board, global_x, global_y);
    barrier(CLK_LOCAL_MEM_FENCE);

    if (inner_cache_x >= 0 && inner_cache_x < kCacheInner && inner_cache_y >= 0 && inner_cache_y < kCacheInner) {
        uchar z = 0;
        int oy = outer_cache_y;
		for (int i = 0; i < 8; ++i) {
          int ox = outer_cache_x*8+i;
		  int neighbours = 0;
		  for (int dx = -1; dx <= +1; ++dx)
			for (int dy = -1; dy <= +1; ++dy) {
              int x = ox + dx;
              int y = oy + dy;
              if (input_cache[x/8][y] & (1 << (x % 8)))
                neighbours++;
            }
          if ((neighbours == 3) || (neighbours == 4 && (input_cache[ox/8][oy] & (1 << (ox % 8)))))
            z |= (1 << (ox % 8));
		}
        new_board[global_x + kByteWidth*global_y] = z;
    }
  }

  kernel void draw_board(global uchar* board, write_only image2d_t board_image) {
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    if (get_cell(board, coord)) {
      write_imagef(board_image, coord, (float4)(0.5, 1.0, 0.5, 1.0));
    } else {
      write_imagef(board_image, coord, (float4)(0.5, 0, 0, 1.0));
    }
  }

)";

template <typename Integral, Integral numerator, Integral denominator>
constexpr Integral checked_div() {
  STATIC_ASSERT(numerator % denominator == 0);
  return numerator / denominator;
}

void Main(const std::vector<string>& args) {
  constexpr size_t kRows = 2048;
  constexpr size_t kBitWidth = kRows;
  constexpr size_t kByteWidth = checked_div<size_t, kBitWidth, 8>();
  constexpr size_t kByteArea = kRows * kByteWidth;
  constexpr size_t kCacheInner = 8;
  constexpr size_t kCacheOuter = kCacheInner + 2;
  constexpr size_t kGroupCols = checked_div<size_t, kByteWidth, kCacheInner>();
  constexpr size_t kGroupRows = checked_div<size_t, kRows, kCacheInner>();

  LOGEXPR(kRows);
  LOGEXPR(kBitWidth);
  LOGEXPR(kByteWidth);
  LOGEXPR(kByteArea);
  LOGEXPR(kCacheInner);
  LOGEXPR(kCacheOuter);
  LOGEXPR(kGroupCols);
  LOGEXPR(kGroupRows);

  sdl::POV pov("cl_texture",
               {
                {SDL_GL_RED_SIZE, 8},
                {SDL_GL_GREEN_SIZE, 8},
                {SDL_GL_BLUE_SIZE, 8},
                {SDL_GL_DEPTH_SIZE, 16},
                {SDL_GL_CONTEXT_MAJOR_VERSION, 4},
                {SDL_GL_CONTEXT_MINOR_VERSION, 3},
                {SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
                {SDL_GL_MULTISAMPLEBUFFERS, 1},
                {SDL_GL_MULTISAMPLESAMPLES, 4},
               },
               0.015 /*uroll*/, 0.05 /*uforward*/, 0.25 /*uright*/);

  auto platform_device_pair = GetGPU();
  compute::platform platform = platform_device_pair.first;
  compute::device device = platform_device_pair.second;

  cl_context_properties properties[] = {
      CL_CONTEXT_PLATFORM, (cl_context_properties)platform.id(),
      CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 0};

  compute::context context(device, properties);
  compute::program board_program =
      compute::program::create_with_source(board_kernel_code, context);
  try {
    board_program.build();
  } catch (boost::compute::opencl_error& e) {
    std::cerr << board_program.build_log() << std::endl;
    throw;
  }

  compute::kernel update_board = board_program.create_kernel("update_board");
  compute::kernel draw_board = board_program.create_kernel("draw_board");

  std::vector<uint8> init_board(kByteArea);
  std::random_device random_device;
  for (uint8& x : init_board) x = random_device();
  compute::buffer board0(context, kByteArea);
  compute::buffer board1(context, kByteArea);

  compute::command_queue command_queue(context, device);

  command_queue.enqueue_write_buffer(board0, 0 /*offset*/, kByteArea,
                                     init_board.data());
  command_queue.finish();

  gl::Texture board_texture;
  gl::Texture::Active(GL_TEXTURE0);
  board_texture.Bind(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl::Texture::Image2D(GL_TEXTURE_2D, 0, GL_RGBA, kBitWidth, kRows, GL_RGBA,
                       GL_UNSIGNED_BYTE, 0);

  compute::opengl_texture board_compute_texture(context, GL_TEXTURE_2D, 0,
                                                board_texture.texture_);

  gl::Shader vertex_shader(GL_VERTEX_SHADER, vertex_shader_code);
  gl::Shader fragment_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

  gl::Program program;
  program.Attach(vertex_shader);
  program.Attach(fragment_shader);

  gl::Attrib position = program.BindAttrib("position");

  program.Link();
  program.Use();

  constexpr float64 period = 1 / 60.0;
  const GLfloat background_color[] = {0, 0, 0};

  gl::Enable(GL_DEPTH_TEST);

  const std::vector<gl::vec4> square = {{-1, -1, 0, 1},
                                        {-1, 1, 0, 1},
                                        {1, -1, 0, 1},
                                        {-1, 1, 0, 1},
                                        {1, -1, 0, 1},
                                        {1, 1, 0, 1}};

  std::vector<gl::vec4> vertexes;

  constexpr int G = 100;

  for (int x = -G; x <= +G; ++x)
    for (int y = -G; y <= +G; ++y)
      for (int i = 0; i < 6; ++i)
        vertexes.emplace_back(square[i](0) + 2 * x, square[i](1) + 2 * y, 0, 1);

  float64 target_timepoint = now_secs();
  float64 start_time = target_timepoint;
  for (int64 frame_num = 0; true; ++frame_num) {
    glClearBufferfv(GL_COLOR, 0, background_color);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (!pov.ProcessEvents()) return;

    update_board.set_arg(frame_num % 2, board0);
    update_board.set_arg((frame_num + 1) % 2, board1);
    command_queue.enqueue_nd_range_kernel(
        update_board, dim(0, 0),
        dim(kGroupCols * kCacheOuter, kGroupRows * kCacheOuter),
        dim(kCacheOuter, kCacheOuter));

    compute::opengl_enqueue_acquire_gl_objects(1, &board_compute_texture.get(),
                                               command_queue);
    draw_board.set_arg(0, (frame_num % 2) ? board1 : board0);
    draw_board.set_arg(1, board_compute_texture);
    command_queue.enqueue_nd_range_kernel(draw_board, dim(0, 0),
                                          dim(kBitWidth, kRows), dim(1, 1));
    compute::opengl_enqueue_release_gl_objects(1, &board_compute_texture.get(),
                                               command_queue);
    command_queue.finish();

    program.Uniform("transform", pov.transform());
    program.Uniform("cat_texture", 0);
    //    program.Uniform("color_frag", vec4(0.2,0.5,0.8,1));
    gl::AttribArrays aa;
    aa.Write(position, vertexes);
    aa.Draw(GL_TRIANGLES);

    pov.Swap();

    target_timepoint += period;
    float64 delay = target_timepoint - now_secs();
    if (delay > 0) SDL_Delay(1000 * delay);

    if ((frame_num & 0x7F) == 0) {
      float64 elapsed = now_secs() - start_time;
      float64 fps = frame_num / elapsed;
      LOGEXPR(fps);
    }
  }
}
