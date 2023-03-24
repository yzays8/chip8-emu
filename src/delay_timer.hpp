#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#include "timer.hpp"

class DelayTimer : public Timer {
 public:
  DelayTimer(uint8_t& dt, std::atomic_bool& is_sleeping);
  ~DelayTimer();
  void Start() override;
  void SetRegisterValue(uint8_t value) override;
  uint8_t& GetRegisterValue() override;
  void Terminate() override;

 private:
  void DecrementTimerValue() override;

  uint8_t& dt_;
  std::mutex mutex_;
  std::thread thread_;
  std::atomic_bool timer_is_running_;
  std::atomic_bool& system_is_sleeping_;
};