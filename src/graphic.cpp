#include <iostream>
#include <array>
#include <SDL2/SDL.h>

#include "graphic.hpp"

Graphic::Graphic()
    : frame_buffer_{}, window_scale_{15}, obj_rgb{0, 255, 255}, bg_rgb{0, 0, 0},
      window_{}, renderer_{}, pixel_{} {
}

Graphic::~Graphic() {
  Terminate();
}

void Graphic::InitializeWindow(const int window_scale) {
  window_scale_ = window_scale;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Failed to initialize SDL graphic" << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }
  if (SDL_CreateWindowAndRenderer(64 * window_scale_, 32 * window_scale_, 0, &window_, &renderer_) != 0) {
    std::cerr << "Failed to create window and renderer" << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }
  SDL_SetWindowTitle(window_, "Chip-8 Emulator");
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
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
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 64; ++j) {
      pixel_.x = window_scale_ * j;
      pixel_.y = window_scale_ * i;
      if (frame_buffer_[i][j] == 1) {
        SDL_SetRenderDrawColor(renderer_, obj_rgb[R], obj_rgb[G], obj_rgb[B], 255);
      } else {
        SDL_SetRenderDrawColor(renderer_, bg_rgb[R], bg_rgb[G], bg_rgb[B], 255);
      }
      SDL_RenderFillRect(renderer_, &pixel_);
    }
  }
  SDL_RenderPresent(renderer_); // This function should not be placed in the loop
}

void Graphic::ChangeObjectColor(uint8_t r, uint8_t g, uint8_t b) {
  obj_rgb[R] = r;
  obj_rgb[G] = g;
  obj_rgb[B] = b;
}

void Graphic::ChangeBackGroundColor(uint8_t r, uint8_t g, uint8_t b) {
  bg_rgb[R] = r;
  bg_rgb[G] = g;
  bg_rgb[B] = b;
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