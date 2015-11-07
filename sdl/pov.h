#pragma once

#include <SDL2/SDL.h>

#include "sdl/application.h"
#include "sdl/glcontext.h"
#include "sdl/window.h"
#include "gl/geometry.h"

namespace sdl {

class POV {
 public:
  POV(const string& name, const std::map<SDL_GLattr, int>& glattributes)
      : forward_direction_(0, 0, -1, 0),
        up_direction_(0, 1, 0, 0),
        right_direction_(1, 0, 0, 0),
        viewer_location_(0, 0, 4) {
    SDL_StopTextInput();
    MUST(SDL_SetRelativeMouseMode(SDL_TRUE) == 0);

    for (const auto& glattribute : glattributes)
      if (SDL_GL_SetAttribute(glattribute.first, glattribute.second) != 0)
        FAIL(SDL_GetError());

    window_.emplace(name.c_str(), SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED, 0 /*w*/, 0 /*h*/,
                    SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    glcontext_.emplace(*window_);

    if (SDL_GL_SetSwapInterval(1) != 0) FAIL(SDL_GetError());
  }

  bool ProcessEvents() {
    float32 dyaw = 0;
    float32 dpitch = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          return false;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
          if (event.key.type == SDL_KEYDOWN &&
              event.key.keysym.sym == SDLK_ESCAPE)
            return false;

          if (event.key.type == SDL_KEYDOWN && event.key.repeat == 0) {
            keys_down_.insert(event.key.keysym.sym);
          }
          if (event.key.type == SDL_KEYUP) {
            keys_down_.erase(event.key.keysym.sym);
          }
          break;

        case SDL_MOUSEMOTION:
          dyaw += float(event.motion.xrel) / 500;
          dpitch -= float(event.motion.yrel) / 500;
          break;
      }
    }

    float32 droll = 0;
    if (keys_down_.count(SDLK_q)) droll -= 0.03;
    if (keys_down_.count(SDLK_e)) droll += 0.03;

    float32 dright = 0;
    if (keys_down_.count(SDLK_a)) dright -= 0.05;
    if (keys_down_.count(SDLK_d)) dright += 0.05;

    float32 dforward = 0;
    if (keys_down_.count(SDLK_w)) dforward += 0.1;
    if (keys_down_.count(SDLK_s)) dforward -= 0.1;

    gl::mat4 dfacing =
        gl::rotate(gl::resize<gl::vec3>(up_direction_), -dyaw) *
        gl::rotate(gl::resize<gl::vec3>(forward_direction_), droll) *
        gl::rotate(gl::resize<gl::vec3>(right_direction_), dpitch);

    up_direction_ = gl::normalize(gl::vec4(dfacing * up_direction_));
    forward_direction_ = gl::normalize(gl::vec4(dfacing * forward_direction_));
    right_direction_ = gl::normalize(gl::vec4(dfacing * right_direction_));

    viewer_location_ += dforward * gl::resize<gl::vec3>(forward_direction_) +
                        dright * gl::resize<gl::vec3>(right_direction_);

    gl::mat4 to_z;
    to_z << 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1;

    gl::mat4 to_camera = gl::mat4::Identity();
    to_camera.col(0) = forward_direction_;
    to_camera.col(1) = up_direction_;
    to_camera.col(2) = right_direction_;

    gl::mat4 world_rot = to_z * gl::transpose(to_camera);

    gl::mat4 world_translate = gl::translate(-viewer_location_);

    world_transform_ = world_rot * world_translate;

    gl::mat4 to_perspective =
        gl::perspective(-0.005 * (1920.0 / 1200), 0.005 * (1920.0 / 1200),
                        0.005, -0.005, 0.01, 100);

    transform_ = to_perspective * world_transform_;
    return true;
  }

  const gl::mat4& world_transform() { return world_transform_; }
  const gl::mat4& transform() { return transform_; }

  void Swap() { window_.value().Swap(); }

 private:
  Application application_;
  optional<Window> window_;
  optional<GLContext> glcontext_;

  gl::vec4 forward_direction_, up_direction_, right_direction_;

  gl::vec3 viewer_location_;

  gl::mat4 world_transform_;
  gl::mat4 transform_;

  std::set<SDL_Keycode> keys_down_;
};

}  // namespace sdl
