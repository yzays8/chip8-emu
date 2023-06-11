#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <atomic>
#include <random>

#include "utils.hpp"
#include "graphic.hpp"
#include "delay_timer.hpp"
#include "sound_timer.hpp"
#include "input.hpp"

namespace chip8_emu {

constexpr int kMainCycles = 500;  // 500 Hz

class Chip8 {
 public:
  Chip8(bool flag_debug);
  void LoadROM(const std::string rom);
  void InitializeWindow(int window_scale);
  bool Run();

 private:
  void StartTimers();
  void Tick();
  void InterpretInstruction(uint16_t inst);
  void Debug(uint16_t inst);

  std::array<uint8_t, 4096> mem_;
  std::array<uint16_t, 16> stack_;
  std::array<uint8_t, 16> v_;
  uint16_t i_;
  uint16_t pc_;
  uint8_t sp_;

  bool flag_debug_;
  bool drawable_;
  std::atomic_bool is_sleeping_;
  bool is_running_;
  bool exit_success_;

  std::unique_ptr<Rand> rand_;
  std::shared_ptr<Graphic> graphic_;
  std::unique_ptr<DelayTimer> delay_timer_;
  std::unique_ptr<SoundTimer> sound_timer_;
  std::unique_ptr<Input> input_;
};

} // namespace chip8_emu
