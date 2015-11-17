#include <map>

#include "core/env.h"
#include "core/must.h"
#include "font/face.h"
#include "font/glyph.h"
#include "main/noargs.h"
#include "sdl/application.h"
#include "sdl/palette.h"
#include "sdl/renderer.h"
#include "sdl/surface.h"
#include "sdl/texture.h"
#include "sdl/window.h"

void Main() {
  sdl::Application application;

  sdl::Window window("render_fonts", SDL_WINDOWPOS_UNDEFINED,
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

  const sdl::Rect viewport = renderer.GetViewport();

  MUST_EQ(viewport.x, 0);
  MUST_EQ(viewport.y, 0);

  int viewport_width = viewport.w;
  int viewport_height = viewport.h;

  sdl::Palette palette(256);
  for (int i = 0; i < 256; ++i)
    palette.SetColor(i, {i / 255.0f, i / 255.0f, i / 255.0f, 1});

  sdl::Surface surface(viewport_width, viewport_height, 8, 0, 0, 0, 0);
  surface.SetPalette(palette);

  MUST_EQ(surface.width(), viewport_width);
  MUST_EQ(surface.height(), viewport_height);

  auto pixels = (uint8*)surface.pixels();

  auto pixel = [&](int w, int h) -> uint8& {
    if (w >= 0 && w < viewport_width && h >= 0 && h < viewport_height) {
      const size_t pos = w + h * viewport_width;
      return pixels[pos];
    } else {
      static uint8 devnull;
      return devnull;
    }
  };

  filesystem::path source_root = GetEnv("SOURCE_ROOT");
  filesystem::path free_serif_ttf =
      source_root / "font" / "testdata" / "FreeSerif.ttf";
  font::Library library;
  font::Face face(library, free_serif_ttf, 0);

  face.SetPixelSize(160, 160);

  float64 frame_checkpoint = now_secs();
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

    for (int w = 0; w < viewport_width; ++w)
      for (int h = 0; h < viewport_height; ++h)
        pixel(w, h) = 128 * (w % 10 == 9 || h % 10 == 9);

    int penx = frame_num % (viewport_width / 3);
    int peny = frame_num % viewport_height;
    optional<FT_UInt> last_glyph_index;
    for (char c : string("Kerning demo: WAY")) {
      auto glyph_index = face.GetCharIndex(c);

      face.LoadGlyph(glyph_index, FT_LOAD_RENDER);
      font::Glyph glyph = face.GetGlyph();

      if (last_glyph_index && (frame_num % 200 < 100)) {
        FT_Vector kerning = face.GetKerning(*last_glyph_index, glyph_index);
        penx += kerning.x >> 6;
        peny -= kerning.y >> 6;
      }
      last_glyph_index = glyph_index;

      const FT_Bitmap& bitmap = glyph.bitmap().bitmap;
      FT_Int x_max = bitmap.width;
      FT_Int y_max = bitmap.rows;

      int ox = penx + glyph.bitmap().left;
      int oy = peny - glyph.bitmap().top;

      for (int x = 0; x < x_max; ++x)
        for (int y = 0; y < y_max; ++y) {
          auto& p = pixel(ox + x, oy + y);
          p = std::max(p, bitmap.buffer[y * x_max + x]);
        }

      penx += glyph.advance().x >> 16;
      peny += glyph.advance().y >> 16;
    }

    sdl::Texture texture(renderer, surface);
    renderer.Render(texture, viewport, viewport);
    renderer.Present();

    {
      const float64 delay = frame_checkpoint - now_secs();

      if (delay > 0) SDL_Delay(delay * 1000 /*ms*/);
    }

    if ((frame_num & 0x7F) == 0) {
      float64 elapsed = now_secs() - start_time;
      float64 fps = frame_num / elapsed;
      LOGEXPR(fps);
    }
  }
}
