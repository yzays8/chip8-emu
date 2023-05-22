#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

constexpr int kDelayTimerCycles = 60; // 60 Hz

class DelayTimer {
 public:
  DelayTimer(std::atomic_bool& is_sleeping);
  ~DelayTimer();
  void Start();
  void SetRegisterValue(uint8_t value);
  uint8_t GetRegisterValue();
  void Terminate();

 private:
  void DecrementTimerValue();

  uint8_t dt_;
  std::mutex mutex_;
  std::thread thread_;
  std::atomic_bool timer_is_running_;
  std::atomic_bool& system_is_sleeping_;
};