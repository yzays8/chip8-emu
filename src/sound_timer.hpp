#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

#include "timer.hpp"
#include "sound.hpp"

class SoundTimer : public Timer {
 public:
  SoundTimer(uint8_t& st, std::shared_ptr<Sound> sound, std::atomic_bool& is_sleeping);
  ~SoundTimer();
  void Start() override;
  void SetRegisterValue(uint8_t value) override;
  uint8_t& GetRegisterValue() override;
  void Terminate() override;

 private:
  void DecrementTimerValue() override;

  uint8_t& st_;
  std::mutex mutex_;
  std::thread thread_;
  std::atomic_bool timer_is_running_;
  bool is_beeping_;
  std::atomic_bool& system_is_sleeping_;
  std::shared_ptr<Sound> sound_;
};