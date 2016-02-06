#include <atomic>
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

class Game {
 public:
  Game(const string& name)
      : window_("snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
                SDL_WINDOW_FULLSCREEN_DESKTOP),
        renderer_(window_),
        quit_(false) {
    SDL_DisplayMode display_mode = window_.GetDisplayMode();
    MUST(display_mode.refresh_rate);
    refresh_period_ = 1.0 / (display_mode.refresh_rate + 1);
    const sdl::Rect viewport = renderer_.GetViewport();
    MUST_EQ(viewport.x, 0);
    MUST_EQ(viewport.y, 0);
    viewport_width_ = viewport.w;
    viewport_height_ = viewport.h;
  }

  void exec() {
    float64 start_time = now_secs();
    float64 checkpoint = start_time;
    for (size_t frame_num = 0; !quit_; ++frame_num) {
      checkpoint += refresh_period_;
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        onEvent(event);
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

      render(renderer_);

      renderer_.Present();

      float64 delay = checkpoint - now_secs();
      if (delay > 0) SDL_Delay(delay * 1000 /*ms*/);
    }
  }

  virtual void render(sdl::Renderer& renderer) = 0;
  virtual void onEvent(const SDL_Event& event) = 0;

  void quit() { quit_ = true; }

  int width() { return viewport_width_; }
  int height() { return viewport_height_; }

 private:
  sdl::Application application_;
  sdl::Window window_;
  sdl::Renderer renderer_;
  int viewport_width_;
  int viewport_height_;
  float64 refresh_period_;
  std::atomic<bool> quit_;
};

constexpr float64 P_food = 0.0001;

class Garden {
 public:
  Garden(int width, int height)
      : width_(width), height_(height), cells_(width * height) {
    posx_ = width / 2;
    posy_ = height / 2;
    len_ = 1;
    set_dir(+1, 0);
    for (int& c : cells_) c = 0;

    cell(posx_, posy_) = 2;
  }

  int& cell(int x, int y) { return cells_[x + y * width_]; }

  void set_dir(int dirx, int diry) {
    MUST(dirx == 0 || diry == 0);
    MUST(dirx + diry == +1 || dirx + diry == -1);
    dirx_ = dirx;
    diry_ = diry;
  }

  bool advance() {
    for (int& c : cells_)
      if (c >= 2) {
        int age = c - 2;
        if (age > len_)
          c = 0;
        else
          c++;
      }
    int newposx = posx_ + dirx_;
    int newposy = posy_ + diry_;
    if (newposx < 0 || newposx >= width_ || newposy < 0 || newposy >= height_)
      return false;
    posx_ = newposx;
    posy_ = newposy;

    int& newcell = cell(newposx, newposy);
    int eaten = newcell;
    newcell = 2;
    if (eaten >= 2) return false;
    if (eaten == 1) len_++;
    for (int& c : cells_)
      if (RandFloat() < P_food && c == 0) c = 1;

    return true;
  }

  int width() { return width_; }
  int height() { return height_; }

 private:
  int posx_, posy_, len_;
  int dirx_, diry_;
  int width_, height_;
  std::vector<int> cells_;
};

class SnakeGame : public Game {
 public:
  SnakeGame() : Game("snake"), garden_(50, 25) {
    cx_ = width() / garden_.width();
    cy_ = height() / garden_.height();
  }

  void render(sdl::Renderer& renderer) override {
    if (tick_++ % 10 == 0)
      if (!garden_.advance()) garden_ = Garden(50, 25);

    renderer.SetDrawColor({0, 0, 0, 1});
    renderer.Clear();
    for (int x = 0; x < garden_.width(); ++x)
      for (int y = 0; y < garden_.height(); ++y) {
        int x1 = x * cx_;
        int y1 = y * cy_;
        int x2 = (x + 1) * cx_;
        int y2 = (y + 1) * cy_;

        int cell = garden_.cell(x, y);
        if (cell == 1) {
          renderer.SetDrawColor({0, 1, 0, 1});
          renderer.DrawRect(x1, y1, x2, y2);
        } else if (cell >= 2) {
          renderer.SetDrawColor({1, 0, 1, 1});
          renderer.DrawRect(x1, y1, x2, y2);
        }
      }
  }

  void onEvent(const SDL_Event& event) override {
    if (event.type != SDL_KEYDOWN || event.key.type != SDL_KEYDOWN) return;

    switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        quit();
        break;

      case SDLK_a:
        garden_.set_dir(-1, 0);
        break;
      case SDLK_s:
        garden_.set_dir(0, +1);
        break;
      case SDLK_d:
        garden_.set_dir(+1, 0);
        break;
      case SDLK_w:
        garden_.set_dir(0, -1);
        break;
    }
  }

 private:
  int64 tick_ = 0;
  Garden garden_;
  int cx_, cy_;
};

void Main() {
  SnakeGame game;
  game.exec();
}

//  sdl::Palette palette1(2);
//  palette1.SetColor(0, {0, 0, 0, 1});
//  palette1.SetColor(1, {1, 1, 1, 1});
//  sdl::Palette palette2(2);
//  palette2.SetColor(0, {0, 0, 0, 1});
//  palette2.SetColor(1, {1, 1, 1, 1});
//
//  sdl::Surface surface1(viewport_width, viewport_height, 1, 0, 0, 0, 0);
//  sdl::Surface surface2(viewport_width, viewport_height, 1, 0, 0, 0, 0);
//  surface1.SetPalette(palette1);
//  surface2.SetPalette(palette2);
//
//  MUST_EQ(surface1.width(), viewport_width);
//  MUST_EQ(surface1.height(), viewport_height);
//
//  sdl::Surface* surface_prev = &surface1;
//  sdl::Surface* surface_next = &surface2;
//  uint8* pixels_prev = (uint8*)surface_prev->pixels();
//  uint8* pixels_next = (uint8*)surface_next->pixels();
//
//  auto set_pixel = [&](uint8* pixels, int w, int h, bool val) {
//    const size_t pos = w + h * viewport_width;
//    uint8& byte = pixels[pos / 8];
//    const uint8 bit = 7 - (pos % 8);
//    if (val)
//      byte |= (1 << bit);
//    else
//      byte &= ~(1 << bit);
//  };
//
//  auto get_pixel = [&](uint8* pixels, int w, int h) -> bool {
//    w = (w + viewport_width) % viewport_width;
//    h = (h + viewport_height) % viewport_height;
//
//    const size_t pos = w + h * viewport_width;
//    const uint8 byte = pixels[pos / 8];
//    const uint8 bit = 7 - (pos % 8);
//    return byte & (1 << bit);
//  };
//
//  for (int w = 0; w < viewport_width; ++w)
//    for (int h = 0; h < viewport_height; ++h)
//      set_pixel(pixels_prev, w, h, RandInt(2));
//
//  for (size_t frame_num = 0; true; ++frame_num) {
//    frame_checkpoint += refresh_period;
//
//    int nalive = 0;
//    for (int w = 0; w < viewport_width; ++w)
//      for (int h = 0; h < viewport_height; ++h) {
//        int neighbours = 0;
//        for (int dw = -1; dw <= +1; ++dw)
//          for (int dh = -1; dh <= +1; ++dh)
//            if (get_pixel(pixels_prev, w + dw, h + dh)) neighbours++;
//        bool live;
//        if (get_pixel(pixels_prev, w, h)) {
//          if (neighbours - 1 < 2)
//            live = false;
//          else if (neighbours - 1 < 4)
//            live = true;
//          else
//            live = false;
//        } else {
//          live = (3 == neighbours);
//        }
//        set_pixel(pixels_next, w, h, live);
//        if (live) ++nalive;
//      }
//
//    if (nalive == 0) {
//      return;
//    }
//
//    std::swap(surface_next, surface_prev);
//    std::swap(pixels_next, pixels_prev);
//
//    sdl::Texture texture(renderer, *surface_prev);
//
//    renderer.Render(texture, viewport, viewport);
//
//    renderer.Present();
//
//    {
//      float64 delay = frame_checkpoint - now_secs();
//
//      if (delay > 0) SDL_Delay(delay * 1000 /*ms*/);
//    }
//
//    if ((frame_num & 0x7F) == 0) {
//      float64 elapsed = now_secs() - start_time;
//      float64 fps = frame_num / elapsed;
//      LOGEXPR(fps);
//    }
//  }
//}
