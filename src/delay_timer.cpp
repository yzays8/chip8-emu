#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include "delay_timer.hpp"

DelayTimer::DelayTimer(std::atomic_bool& is_sleeping)
    : dt_{0},
      mutex_{},
      thread_{},
      timer_is_running_{false},
      system_is_sleeping_{is_sleeping} {
}

DelayTimer::~DelayTimer() {
  if (timer_is_running_) {
    Terminate();
  }
}

void DelayTimer::Start() {
  const auto interval = std::chrono::duration<int, std::ratio<1, kDelayTimerCycles>>(1); // 60 Hz
  timer_is_running_ = true;
  thread_ = std::thread([this, interval] {
    while (timer_is_running_) {
      std::this_thread::sleep_for(interval);
      if (system_is_sleeping_) {
        // stop timer
        continue;
      }
      DecrementTimerValue();
    }
  });
}

void DelayTimer::DecrementTimerValue() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (dt_ > 0) {
    --dt_;
  }
}

void DelayTimer::SetRegisterValue(uint8_t value) {
  std::lock_guard<std::mutex> lock(mutex_);
  dt_ = value;
}

uint8_t DelayTimer::GetRegisterValue() {
  std::lock_guard<std::mutex> lock(mutex_);
  return dt_;
}

void DelayTimer::Terminate() {
  timer_is_running_ = false;
  thread_.join();
  std::cout << "Stopped DelayTimer" << std::endl;
}