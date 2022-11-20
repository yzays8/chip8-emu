#ifndef CHIP8_H_
#define CHIP8_H_

#include <cstdint>
#include <array>

class Chip8 {
 public:
  Chip8();
  std::array<std::array<int, 64>, 32> frameBuffer;
  bool key[16];
  bool drawable{false};

  void LoadROM(const std::string rom);
  void InterpretInstruction(const uint16_t ins);
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
};

#endif