#pragma once

#include <cstdint>
#include <random>

namespace chip8_emu {

class Rand {
 public:
  Rand();
  uint8_t GetRandomByte();  // return [0, 255]

 private:
  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_int_distribution<> dis_;
};

} // namespace chip8_emu
