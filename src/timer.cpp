#include "timer.hpp"

Timer::~Timer() {
}

void Timer::TimeoutHandler() {
  DecrementTimerValue();
}