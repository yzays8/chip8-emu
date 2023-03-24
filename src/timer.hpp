#pragma once

#include <cstdint>

constexpr int kTimerCycles = 60; // 60 Hz

class Timer {
 public:
  virtual ~Timer() = 0;
  virtual void Start() = 0;
  virtual void SetRegisterValue(uint8_t value) = 0;
  virtual uint8_t& GetRegisterValue() = 0;
  virtual void Terminate() = 0;

 protected:
  void TimeoutHandler();
  virtual void DecrementTimerValue() = 0;
};