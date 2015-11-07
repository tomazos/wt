#pragma once

#include <SDL2/SDL.h>

#include "gl/geometry.h"
#include "sdl/rect.h"
#include "sdl/window.h"

namespace sdl {

class Texture;

class Renderer {
 public:
  static void LogDriversInfo();

  Renderer(Window& window);

  Rect GetViewport();

  void Clear();

  void SetDrawColor(gl::vec4 rgba);

  void DrawLine(int x1, int y1, int x2, int y2);

  void Render(Texture& texture, const optional<Rect>& src,
              const optional<Rect>& dest);

  void Present();

 private:
  std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)> renderer_;

  friend class Texture;
};

}  // namespace sdl
