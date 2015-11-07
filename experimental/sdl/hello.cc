#include <SDL2/SDL.h>

#include <cstring>
#include <map>
#include <set>
#include <sys/time.h>

#include "core/must.h"
#include "core/random.h"
#include "gl/attrib_arrays.h"
#include "gl/program.h"
#include "gl/shader.h"
#include "gl/system.h"
#include "gl/geometry.h"
#include "sdl/pov.h"

using namespace gl;

constexpr auto vertex_shader_code = R"(

  #version 430 core

  #define M_PI 3.1415926535897932384626433832795

  out vec4 color_thru;
  in vec4 position;
  in vec4 normal_in;
  uniform mat4 transform;
  uniform mat4 inverse_transform;
  uniform vec4 light;
  uniform vec3 color;

  void main(void)
  {
    vec4 normal = normalize(inverse_transform * normal_in);
    gl_Position = transform * position;
    float col = 0.3*(dot(normal.xyz,vec3(sin(0.1),cos(0.1),0)));
    color_thru.rgb = 0.5*color + vec3(col,col,col);
    color_thru.a = 1;
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

void Main(const std::vector<string>& args) {
  int tesselation_count = std::stoi(args.at(0));
  int num_spheres = std::stoi(args.at(1));

  sdl::POV pov("hello",
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

  Shader vertex_shader(GL_VERTEX_SHADER, vertex_shader_code);
  Shader fragment_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

  Program program;
  program.Attach(vertex_shader);
  program.Attach(fragment_shader);

  Attrib position = program.BindAttrib("position");
  Attrib color = program.BindAttrib("color");
  Attrib normal = program.BindAttrib("normal_in");

  program.Link();
  program.Use();

  constexpr float64 period = 1 / 60.0;
  const GLfloat background_color[] = {0, 0, 0};
  float64 target_timepoint = now_secs();

  std::set<SDL_Keycode> keys;

  Enable(GL_DEPTH_TEST);

  std::vector<vec4> vertexes;
  std::vector<vec4> normals;

  for (auto v3 : sphere(tesselation_count)) {
    vertexes.push_back(resize<vec4>(v3.cast<float32>()) + vec4(0, 0, 0, 1));
  }

  for (size_t i = 0; i < vertexes.size() / 3; ++i) {
    vec3 v0 = resize<vec3>(vertexes[3 * i + 0]);
    vec3 v1 = resize<vec3>(vertexes[3 * i + 1]);
    vec3 v2 = resize<vec3>(vertexes[3 * i + 2]);
    vec3 normal3 = normalize(vec3((v0 + v1 + v2) / 3));
    vec4 normal4;
    normal4.topRows(3) = normal3;
    normal4(3) = 0;
    normals.push_back(normal4);
    normals.push_back(normal4);
    normals.push_back(normal4);
  }

  std::vector<vec3> axises;
  std::vector<vec3> offsets;
  std::vector<float64> speeds;
  std::vector<vec3> colors;

  for (int i = 0; i < num_spheres; ++i) {
    vec3 axis = normalize(vec3(RandFloat(), RandFloat(), RandFloat()));
    vec3 offset(RandFloat() * 10, RandFloat() * 10, RandFloat() * 10);
    axises.push_back(axis);
    offsets.push_back(offset);
    speeds.push_back(RandFloat());
    colors.emplace_back(RandFloat(), RandFloat(), RandFloat());
  }

  float64 start_timepoint = target_timepoint;

  for (int64 frame_num = 0; true; ++frame_num) {
    glClearBufferfv(GL_COLOR, 0, background_color);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (!pov.ProcessEvents()) return;

    for (int i = 0; i < num_spheres; ++i) {
      mat4 spin_transform =
          rotate(axises[i], speeds[i] * (target_timepoint - start_timepoint)) *
          translate(offsets[i]);

      mat4 transform(pov.transform() * spin_transform);

      program.Uniform("transform", transform);

      mat4 world_transform = pov.world_transform() * spin_transform;

      mat3 world_transform3 = resize<mat3>(world_transform);

      mat3 inverse_transform3 = transpose(world_transform3.inverse());

      mat4 inverse_transform4 = resize<mat4>(inverse_transform3);

      program.Uniform("inverse_transform", inverse_transform4);

      program.Uniform("color", colors[i]);

      AttribArrays aa;
      aa.Write(position, vertexes);
      aa.Write(normal, normals);
      aa.Draw(GL_TRIANGLES);
    }
    pov.Swap();

    target_timepoint += period;
    float64 delay = target_timepoint - now_secs();
    if (delay > 0) SDL_Delay(1000 * delay);
  }
}
