#include <map>

#include "core/must.h"
#include "main/noargs.h"
#include "sdl/application.h"
#include "sdl/renderer.h"
#include "sdl/surface.h"
#include "sdl/texture.h"
#include "sdl/window.h"

void Main() {
  sdl::Application application;

  sdl::Window window("render_image", SDL_WINDOWPOS_UNDEFINED,
                     SDL_WINDOWPOS_UNDEFINED, 0, 0,
                     SDL_WINDOW_FULLSCREEN_DESKTOP);

  sdl::Renderer::LogDriversInfo();

  sdl::Renderer renderer(window);

  SDL_Event event;

  const float64 start_time = now_secs();

  SDL_DisplayMode display_mode = window.GetDisplayMode();
  MUST(display_mode.refresh_rate);
  LOGEXPR(display_mode.w);
  LOGEXPR(display_mode.h);
  LOGEXPR(display_mode.refresh_rate);
  const float64 refresh_period = 1.0 / (display_mode.refresh_rate + 1);
  LOGEXPR(refresh_period);

  float64 frame_checkpoint = now_secs();

  const sdl::Rect viewport = renderer.GetViewport();

  MUST_EQ(viewport.x, 0);
  MUST_EQ(viewport.y, 0);

  int viewport_width = viewport.w;
  int viewport_height = viewport.h;

  sdl::Surface cat_surface("/home/zos/data/cat.bmp");
  sdl::Texture cat_texture(renderer, cat_surface);

  const sdl::Rect cat_dimensions = cat_texture.Dimensions();
  int cat_width = cat_dimensions.w;
  int cat_height = cat_dimensions.h;

  int margin_width = viewport_width - cat_width;
  int margin_height = viewport_height - cat_height;
  MUST_GE(margin_width, 0);
  MUST_GE(margin_height, 0);

  int posx = 0;
  int posy = 0;
  int dirx = +1;
  int diry = +1;

  for (size_t frame_num = 0; true; ++frame_num) {
    MUST_GE(posx, 0);
    MUST_LT(posx, margin_width);
    MUST_GE(posy, 0);
    MUST_LT(posy, margin_height);

    if (posx == 0) dirx = +1;
    if (posx == margin_width - 1) dirx = -1;
    if (posy == 0) diry = +1;
    if (posy == margin_height - 1) diry = -1;

    frame_checkpoint += refresh_period;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          LOG("SDL_QUIT");
          return;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
          if (event.key.type == SDL_KEYDOWN &&
              event.key.keysym.sym == SDLK_ESCAPE) {
            LOG("ESCAPE");
            return;
          }
      }
    }

    renderer.SetDrawColor({1, 1, 1, 1});
    renderer.Clear();

    renderer.Render(cat_texture, cat_dimensions,
                    sdl::Rect{posx, posy, cat_width, cat_height});

    renderer.Present();

    {
      float64 delay = frame_checkpoint - now_secs();

      if (delay > 0) SDL_Delay(delay * 1000 /*ms*/);
    }

    if ((frame_num & 0x7F) == 0) {
      float64 elapsed = now_secs() - start_time;
      float64 fps = frame_num / elapsed;
      LOGEXPR(fps);
    }

    posx += dirx;
    posy += diry;
  }
}
