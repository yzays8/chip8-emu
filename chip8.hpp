#ifndef CHIP8_H_
#define CHIP8_H_

#include <cstdint>
#include <array>

#include <SDL2/SDL.h>

class Chip8 {
 public:
  Chip8();
  std::array<std::array<int, 64>, 32> frame_buffer;
  bool key[16];
  bool drawable{false};
  bool sleep{false};

  void LoadROM(const std::string rom);
  void InterpretInstruction(const uint16_t inst);
  void RunLoop();

 private:
  uint8_t mem_[4096];
  uint16_t stack_[16];
  uint8_t v_[16];
  uint16_t i_;
  uint16_t pc_;
  uint8_t sp_;
  uint8_t dt_;
  uint8_t st_;
  Uint32 main_clock_ticks_;
};

#endif