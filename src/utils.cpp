#include <cstdint>
#include <random>

#include "utils.hpp"

namespace chip8_emu {

Rand::Rand() : rd_{}, gen_{rd_()}, dis_{0, 255} {}

uint8_t Rand::GetRandomByte() {
  return static_cast<uint8_t>(dis_(gen_));
}

} // namespace chip8_emu
