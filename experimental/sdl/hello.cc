#include <cstring>
#include <map>
#include <sys/time.h>

#include "core/must.h"
#include "core/random.h"
#include "experimental/sdl/buffer.h"
#include "experimental/sdl/error.h"
#include "experimental/sdl/opengl.h"
#include "experimental/sdl/program.h"
#include "experimental/sdl/shader.h"
#include "experimental/sdl/vertex_array.h"
#include <SDL2/SDL.h>

static float64 now() {
  timeval tv;
  std::memset(&tv, 0, sizeof tv);
  int gettimeofday_result = gettimeofday(&tv, nullptr);
  if (gettimeofday_result != 0) THROW_ERRNO("gettimeofday");
  return float64(tv.tv_sec) + float64(tv.tv_usec) / 1'000'000;
}

constexpr int kScreenWidth = 1920;
constexpr int kScreenHeight = 1200;

using namespace gl;

struct Particle {
  vec3 orbits;
  vec3 start;
  vec3 toward;
  float speed;

  std::vector<vec3> gen_triangles {
    vec3 radius =
  }
};

Particle RandParticle() {
  vec3 orbits(RandFloat() * 2 - 1, RandFloat() * 2 - 1, RandFloat() * 2 - 1);
  vec3 start(RandFloat() * 2 - 1, RandFloat() * 2 - 1, RandFloat() * 2 - 1);
  vec3 toward(RandFloat() * 2 - 1, RandFloat() * 2 - 1, RandFloat() * 2 - 1);
  float speed = RandFloat();

  return Particle{orbits, start, toward, speed};
}

vec4 v[3] = {
    {-0.5, 0.5, 0, 1}, {-0.5, -0.5, 0, 1}, {0.5, 0, 0, 1},
};

constexpr auto vertex_shader_code = R"(

  #version 430 core

  #define M_PI 3.1415926535897932384626433832795

  out vec4 color_thru;
  in vec3 position;

  void main(void)
  {
    gl_Position = vec4(position.x, position.y, position.z, 1);
    color_thru = vec4(0.5,0.1,0.9,0);
  }

)";

constexpr auto fragment_shader_code = R"(

  #version 430 core

  in vec4 color_thru;
  out vec4 color_out;

  void main(void)
  {
    color_out = color_thru;
  }

)";

namespace sdl {

struct Application {
  Application() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) FAIL(SDL_GetError());
  }
  ~Application() { SDL_Quit(); }
};

struct Window {
  Window()
      : window_(SDL_CreateWindow("hello", SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED, kScreenWidth,
                                 kScreenHeight,
                                 SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL)) {
    MUST(window_);
  }

  void Swap() { SDL_GL_SwapWindow(window_); }

  ~Window() { SDL_DestroyWindow(window_); }
  SDL_Window* window_;
};

struct GLContext {
  GLContext(Window& window) : context_(SDL_GL_CreateContext(window.window_)) {
    MUST(context_ != nullptr);
  }
  ~GLContext() { SDL_GL_DeleteContext(context_); }

  SDL_GLContext context_;
};
}

void Main(const std::vector<string>& args) {
  float particle_size = std::stof(args.at(0));
  int particle_count = std::stoi(args.at(1));

  std::vector<Particle> particles;
  for (int i = 0; i < particle_count; ++i) {
    particles.push_back(RandParticle());
  }

  sdl::Application application;

  std::map<SDL_GLattr, int> glattributes = {
      {SDL_GL_RED_SIZE, 8},
      {SDL_GL_GREEN_SIZE, 8},
      {SDL_GL_BLUE_SIZE, 8},
      {SDL_GL_CONTEXT_MAJOR_VERSION, 4},
      {SDL_GL_CONTEXT_MINOR_VERSION, 3},
      {SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
  };

  for (const auto& glattribute : glattributes)
    if (SDL_GL_SetAttribute(glattribute.first, glattribute.second) != 0)
      FAIL(SDL_GetError());

  sdl::Window window;
  sdl::GLContext context(window);

  std::vector<vec3> vertexes;

  for (const Particle& particle : particles) {

  }

  Buffer vertex_buffer(vertexes.data(),
                                 vertexes.size() * sizeof(vec3),
                                 GL_ARRAY_BUFFER, GL_STATIC_DRAW);

  if (SDL_GL_SetSwapInterval(1) != 0) FAIL(SDL_GetError());

  LOGEXPR(glGetString(GL_VENDOR));
  LOGEXPR(glGetString(GL_RENDERER));
  LOGEXPR(glGetString(GL_VERSION));
  LOGEXPR(glGetString(GL_SHADING_LANGUAGE_VERSION));
  int max_attribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
  LOGEXPR(max_attribs);
  Shader vertex_shader(GL_VERTEX_SHADER, vertex_shader_code);
  Shader fragment_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

  Program program;
  program.Attach(vertex_shader);
  program.Attach(fragment_shader);

  Attrib position = program.BindAttrib("position");

  program.Link();
  program.Use();
  program.Uniform("particle_size", particle_size);

  VertexArray va;
  va.EnableAttribArray(position);
  va.BindAttrib(position, 5 /*bindingindex*/);
  va.AttribFormat(position, 3, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
  va.BindBuffer(5 /*bindingindex*/, vertex_buffer, 0 /*offset*/, sizeof(vec3));

  constexpr float64 period = 1 / 60.0;
  float64 target_timepoint = now();
  for (size_t i = 0; i < 180; ++i) {
    const GLfloat background_color[] = {
        float(0.5f + 0.5f * sin(target_timepoint)),
        float(0.5f + 0.5f * sin(target_timepoint * 2)),
        float(0.5f + 0.5f * sin(target_timepoint / 2)), 0.0f};
    glClearBufferfv(GL_COLOR, 0, background_color);
    program.Uniform("now", now());

    va.Draw(GL_TRIANGLES, 0, 3);

    window.Swap();

    target_timepoint += period;
    float64 delay = target_timepoint - now();
    if (delay > 0) SDL_Delay(1000 * delay);
  }
}
