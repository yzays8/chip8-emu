#pragma once

#include <cstdint>
#include <array>

#include <SDL2/SDL.h>

namespace chip8_emu {

extern const std::array<uint8_t, 80> kSprites;

struct Color {
  uint8_t r, g, b;
};

class Graphic {
 public:
  Graphic();
  ~Graphic();
  void InitializeWindow(int window_scale);
  void Render();
  void ChangeObjectColor(Color color);
  void ChangeBackGroundColor(Color color);
  void Terminate();
  std::array<std::array<bool, 64>, 32>& GetBuffer();

 private:
  std::array<std::array<bool, 64>, 32> frame_buffer_;
  int window_scale_;
  Color obj_rgb_, bg_rgb_;
  SDL_Window *window_;
  SDL_Renderer *renderer_;
  SDL_Rect pixel_;
};

} // namespace chip8_emu
