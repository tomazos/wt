#include <SDL2/SDL.h>

#include <cstring>
#include <map>
#include <set>
#include <sys/time.h>

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

using namespace gl;

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

void Main(const std::vector<string>& args) {
  sdl::POV pov("cat_texture",
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

  sdl::Surface cat_surface("/home/zos/data/cat.bmp");
  LOGEXPR(sdl::PixelFormatEnumToString(cat_surface.format().format));
  LOGEXPR(cat_surface.format().BitsPerPixel);
  LOGEXPR(cat_surface.format().BytesPerPixel);
  LOGEXPR(cat_surface.format().BytesPerPixel);
  LOGEXPR(cat_surface.width());
  LOGEXPR(cat_surface.height());
  LOGEXPR(cat_surface.pitch());

  gl::Texture texture;
  gl::Texture::Active(GL_TEXTURE0 + 42);
  texture.Bind(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  gl::Texture::Image2D(GL_TEXTURE_2D, 0, GL_RGB, cat_surface.width(),
                       cat_surface.height(), GL_RGB, GL_UNSIGNED_BYTE,
                       cat_surface.pixels());
  gl::Sampler sampler;
  sampler.Bind(42);

  Shader vertex_shader(GL_VERTEX_SHADER, vertex_shader_code);
  Shader fragment_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

  Program program;
  program.Attach(vertex_shader);
  program.Attach(fragment_shader);

  Attrib position = program.BindAttrib("position");

  program.Link();
  program.Use();

  constexpr float64 period = 1 / 60.0;
  const GLfloat background_color[] = {1, 1, 1};

  Enable(GL_DEPTH_TEST);

  std::vector<vec4> vertexes = {{-1, -1, 0, 1},
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

    program.Uniform("transform", pov.transform());
    program.Uniform("cat_texture", 42);
    //    program.Uniform("color_frag", vec4(0.2,0.5,0.8,1));
    AttribArrays aa;
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
