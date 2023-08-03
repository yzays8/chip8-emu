#include <iostream>
#include <array>

#include <SDL2/SDL.h>

#include "graphic.hpp"

namespace chip8_emu {

const std::array<uint8_t, 80> kSprites = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

Graphic::Graphic()
    : frame_buffer_{},
      window_scale_{15},
      obj_rgb_{255, 255, 255},
      bg_rgb_{0, 0, 0},
      window_{nullptr},
      renderer_{nullptr},
      pixel_{} {
}

Graphic::~Graphic() {
  Terminate();
}

void Graphic::InitializeWindow(int window_scale) {
  window_scale_ = window_scale;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Failed to initialize SDL graphic: " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }

  if (SDL_CreateWindowAndRenderer(64 * window_scale_, 32 * window_scale_, 0, &window_, &renderer_) != 0) {
    std::cerr << "Failed to create SDL window or SDL renderer: " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }
  SDL_SetWindowTitle(window_, "CHIP-8 Emulator");

  SDL_SetRenderDrawColor(renderer_, bg_rgb_.r, bg_rgb_.g, bg_rgb_.b, 255);
  SDL_RenderClear(renderer_);
  SDL_RenderPresent(renderer_);
  pixel_ = {
    0,              // x (temporary)
    0,              // y (temporary)
    window_scale_,  // width
    window_scale_,  // height
  };

  std::cout << "Initialized window" << std::endl;
}

void Graphic::Render() {
  // draw background
  SDL_SetRenderDrawColor(renderer_, bg_rgb_.r, bg_rgb_.g, bg_rgb_.b, 255);
  SDL_RenderClear(renderer_);

  // draw objects
  SDL_SetRenderDrawColor(renderer_, obj_rgb_.r, obj_rgb_.g, obj_rgb_.b, 255);
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 64; ++j) {
      if (frame_buffer_[i][j] == 1) {
        pixel_.x = window_scale_ * j;
        pixel_.y = window_scale_ * i;
        SDL_RenderFillRect(renderer_, &pixel_);
      }
    }
  }
  SDL_RenderPresent(renderer_); // This function should not be placed in the loop
}

void Graphic::ChangeObjectColor(Color color) {
  obj_rgb_ = color;
}

void Graphic::ChangeBackGroundColor(Color color) {
  bg_rgb_ = color;
}

std::array<std::array<bool, 64>, 32>& Graphic::GetBuffer() {
  return frame_buffer_;
}

void Graphic::Terminate() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
  std::cout << "Closed window" << std::endl;
}

} // namespace chip8_emu
