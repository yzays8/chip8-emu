#pragma once

#include <cstdint>
#include <array>
#include <SDL2/SDL.h>

const uint8_t kSprites[80] = {
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

class Chip8 {
 public:
  Chip8();
  void LoadROM(const std::string rom);
  void InitializeWindow(const int window_scale);
  void RunLoop();

 private:
  void ProcessInput();
  void Tick();
  void Render();
  void InterpretInstruction(const uint16_t inst);
  void Debug(const uint16_t inst);

  std::array<uint8_t, 4096> mem_;
  std::array<uint16_t, 16> stack_;
  std::array<uint8_t, 16> v_;
  uint16_t i_;
  uint16_t pc_;
  uint8_t sp_;
  uint8_t dt_;
  uint8_t st_;

  Uint32 main_clock_ticks_;
  std::array<std::array<int, 64>, 32> frame_buffer_;
  std::array<bool, 16> key_;
  bool drawable_;
  bool sleep_;

  SDL_Window *window_;
  SDL_Renderer *renderer_;
  int window_scale_;
  SDL_Rect pixel_;
};