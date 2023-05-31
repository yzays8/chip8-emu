#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <atomic>
#include <random>

#include "graphic.hpp"
#include "delay_timer.hpp"
#include "sound_timer.hpp"
#include "input.hpp"

namespace chip8_emu {

constexpr int kMainCycles = 500;  // 500 Hz

class Chip8 {
 public:
  Chip8(const bool flag_debug);
  void LoadROM(const std::string rom);
  void InitializeWindow(const int window_scale);
  void RunLoop();

 private:
  void StartTimers();
  void Tick();
  void InterpretInstruction(const uint16_t inst);
  void Debug(const uint16_t inst);

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

  std::shared_ptr<Graphic> graphic_;
  std::unique_ptr<DelayTimer> delay_timer_;
  std::unique_ptr<SoundTimer> sound_timer_;
  std::unique_ptr<Input> input_;

  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_int_distribution<> dis_;
};

} // namespace chip8_emu
