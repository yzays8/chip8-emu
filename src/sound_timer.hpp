#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

#include "sound.hpp"

namespace chip8_emu {

constexpr int kSoundTimerCycles = 60; // 60 Hz

class SoundTimer {
 public:
  SoundTimer(std::atomic_bool& is_sleeping, const std::string& beep_file_path);
  ~SoundTimer();
  void Start();
  void SetRegisterValue(uint8_t value);
  uint8_t GetRegisterValue();
  void Terminate();

 private:
  void DecrementTimerValue();

  uint8_t st_;
  std::mutex mutex_;
  std::thread thread_;
  std::atomic_bool timer_is_running_;
  bool is_beeping_;
  std::atomic_bool& system_is_sleeping_;
  std::unique_ptr<Sound> sound_;
  std::string beep_file_path_;
};

} // namespace chip8_emu
