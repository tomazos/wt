#include <map>

#include "core/must.h"
#include "core/random.h"
#include "main/noargs.h"
#include "sdl/application.h"
#include "sdl/palette.h"
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

  sdl::Palette palette1(2);
  palette1.SetColor(0, {0, 0, 0, 1});
  palette1.SetColor(1, {1, 1, 1, 1});

  sdl::Palette palette2(2);
  palette2.SetColor(0, {0, 0, 0, 1});
  palette2.SetColor(1, {1, 1, 1, 1});

  sdl::Surface surface1(viewport_width, viewport_height, 1, 0, 0, 0, 0);
  sdl::Surface surface2(viewport_width, viewport_height, 1, 0, 0, 0, 0);
  surface1.SetPalette(palette1);
  surface2.SetPalette(palette2);

  MUST_EQ(surface1.width(), viewport_width);
  MUST_EQ(surface1.height(), viewport_height);

  sdl::Surface* surface_prev = &surface1;
  sdl::Surface* surface_next = &surface2;
  uint8* pixels_prev = (uint8*)surface_prev->pixels();
  uint8* pixels_next = (uint8*)surface_next->pixels();

  auto set_pixel = [&](uint8* pixels, int w, int h, bool val) {
    const size_t pos = w + h * viewport_width;
    uint8& byte = pixels[pos / 8];
    const uint8 bit = 7 - (pos % 8);
    if (val)
      byte |= (1 << bit);
    else
      byte &= ~(1 << bit);
  };

  auto get_pixel = [&](uint8* pixels, int w, int h) -> bool {
    w = (w + viewport_width) % viewport_width;
    h = (h + viewport_height) % viewport_height;

    const size_t pos = w + h * viewport_width;
    const uint8 byte = pixels[pos / 8];
    const uint8 bit = 7 - (pos % 8);
    return byte & (1 << bit);
  };

  for (int w = 0; w < viewport_width; ++w)
    for (int h = 0; h < viewport_height; ++h)
      set_pixel(pixels_prev, w, h, RandInt(2));

  for (size_t frame_num = 0; true; ++frame_num) {
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

    int nalive = 0;
    for (int w = 0; w < viewport_width; ++w)
      for (int h = 0; h < viewport_height; ++h) {
        int neighbours = 0;
        for (int dw = -1; dw <= +1; ++dw)
          for (int dh = -1; dh <= +1; ++dh)
            if (get_pixel(pixels_prev, w + dw, h + dh)) neighbours++;
        bool live;
        if (get_pixel(pixels_prev, w, h)) {
          if (neighbours - 1 < 2)
            live = false;
          else if (neighbours - 1 < 4)
            live = true;
          else
            live = false;
        } else {
          live = (3 == neighbours);
        }
        set_pixel(pixels_next, w, h, live);
        if (live) ++nalive;
      }

    if (nalive == 0) {
      return;
    }

    std::swap(surface_next, surface_prev);
    std::swap(pixels_next, pixels_prev);

    sdl::Texture texture(renderer, *surface_prev);

    renderer.Render(texture, viewport, viewport);

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
  }
}
