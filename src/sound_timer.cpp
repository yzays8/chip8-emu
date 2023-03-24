#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include "sound_timer.hpp"
#include "sound.hpp"

SoundTimer::SoundTimer(uint8_t& st, std::shared_ptr<Sound> sound, std::atomic_bool& is_sleeping)
    : st_{st}, mutex_{}, thread_{}, timer_is_running_{false}, is_beeping_{false},
      system_is_sleeping_{is_sleeping}, sound_{sound} {
}

SoundTimer::~SoundTimer() {
  if (timer_is_running_) {
    Terminate();
  }
}

void SoundTimer::Start() {
  const auto interval = std::chrono::duration<int, std::ratio<1, kTimerCycles>>(1); // 60 Hz
  timer_is_running_ = true;
  thread_ = std::thread([this, interval]{
    while (timer_is_running_) {
      std::this_thread::sleep_for(interval);
      if (system_is_sleeping_) {
        // stop timer
        sound_->StopBeep();
        continue;
      } else if (is_beeping_) {
        // resume timer
        sound_->Beep();
      }
      TimeoutHandler();
    }
  });
}

void SoundTimer::DecrementTimerValue() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (st_ > 0) {
    if (!is_beeping_) {
      sound_->Beep();
      is_beeping_ = true;
    }
    --st_;
  } else if (is_beeping_) {
    sound_->StopBeep();
    is_beeping_ = false;
  }
}

void SoundTimer::SetRegisterValue(uint8_t value) {
  std::lock_guard<std::mutex> lock(mutex_);
  st_ = value;
}

uint8_t& SoundTimer::GetRegisterValue() {
  std::lock_guard<std::mutex> lock(mutex_);
  return st_;
}

void SoundTimer::Terminate() {
  timer_is_running_ = false;
  thread_.join();
  std::cout << "Stopped SoundTimer" << std::endl;
}