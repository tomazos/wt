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

  constant int W = 128;

  bool get_cell(global uchar* board, int2 coord) {
    int pos = coord.x + coord.y*W;
    int byte_pos = pos / 8;
    int bit_pos = (1 << (pos %8));
    uchar byte_val = board[byte_pos];
    if ((byte_val & bit_pos) != 0)
      return true;
    else
      return false;
  }

  void set_cell(global uchar* board, int2 coord, bool val) {
    int pos = coord.x + coord.y*W;
    if (val) {
      board[pos / 8] |= (1 << (pos % 8));
    } else {
      board[pos / 8] &= ~(1 << (pos % 8));
    }
  }
  
  kernel void init_board(global uchar* board) {
    for (int i = 0; i < 8; ++i) {
      int2 coord = (int2)(get_global_id(0)*8 + i, get_global_id(1));
      set_cell(board, coord, coord.x % 2 == 0 || coord.y % 2 == 0);
    }
  }

  kernel void update_board(global uchar* board) {
  }

  kernel void draw_board(global uchar* board, write_only image2d_t board_image) {
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    if (get_cell(board, coord)) {
      write_imagef(board_image, coord, (float4)(255, 255, 255, 255));
    } else {
      write_imagef(board_image, coord, (float4)(0, 0, 0, 255));
    }
  }

)";

void Main(const std::vector<string>& args) {
  constexpr size_t W = 128;
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
               });

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

  compute::kernel init_board = board_program.create_kernel("init_board");
  compute::kernel update_board = board_program.create_kernel("update_board");
  compute::kernel draw_board = board_program.create_kernel("draw_board");

  compute::buffer board(context, W * W / 8);

  compute::command_queue command_queue(context, device);

  init_board.set_arg(0, board);
  command_queue.enqueue_nd_range_kernel(init_board, dim(0, 0), dim(W / 8, W),
                                        dim(1, 1));
  command_queue.finish();

  gl::Texture board_texture;
  gl::Texture::Active(GL_TEXTURE0);
  board_texture.Bind(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl::Texture::Image2D(GL_TEXTURE_2D, 0, GL_RGBA, W, W, GL_RGBA,
                       GL_UNSIGNED_BYTE, 0);

  compute::opengl_texture board_compute_texture(context, GL_TEXTURE_2D, 0,
                                                board_texture.texture_);

  compute::opengl_enqueue_acquire_gl_objects(1, &board_compute_texture.get(),
                                             command_queue);
  draw_board.set_arg(0, board);
  draw_board.set_arg(1, board_compute_texture);
  command_queue.enqueue_nd_range_kernel(draw_board, dim(0, 0), dim(W, W),
                                        dim(1, 1));
  compute::opengl_enqueue_release_gl_objects(1, &board_compute_texture.get(),
                                             command_queue);
  command_queue.finish();

  gl::Shader vertex_shader(GL_VERTEX_SHADER, vertex_shader_code);
  gl::Shader fragment_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

  gl::Program program;
  program.Attach(vertex_shader);
  program.Attach(fragment_shader);

  gl::Attrib position = program.BindAttrib("position");

  program.Link();
  program.Use();

  constexpr float64 period = 1 / 60.0;
  const GLfloat background_color[] = {1, 1, 1};

  gl::Enable(GL_DEPTH_TEST);

  std::vector<gl::vec4> vertexes = {{-1, -1, 0, 1},
                                    {-1, 1, 0, 1},
                                    {1, -1, 0, 1},
                                    {-1, 1, 0, 1},
                                    {1, -1, 0, 1},
                                    {1, 1, 0, 1}};

  float64 target_timepoint = now_secs();
  float64 start_time = target_timepoint;
  for (int64 frame_num = 0; true; ++frame_num) {
    glClearBufferfv(GL_COLOR, 0, background_color);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (!pov.ProcessEvents()) return;

    //    std::vector<cl::Memory> globjects{image};
    //    command_queue.enqueueAcquireGLObjects(&globjects);

    //    kernel_functor(cl::EnqueueArgs(command_queue, cl::NDRange(0, 0),
    //                                   cl::NDRange(100, 200), cl::NDRange(1,
    //                                   1)),
    //                   0.5 + 0.5 * sin(target_timepoint), image);

    //    command_queue.enqueueReleaseGLObjects(&globjects);
    //    command_queue.finish();

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
