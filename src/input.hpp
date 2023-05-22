#pragma once

#include <array>
#include <atomic>
#include <memory>

#include "graphic.hpp"

enum MessageType {
  MSG_NONE,
  MSG_CHANGE_SLEEP_STATE,
  MSG_SHUTDOWN,
};

class Input {
 public:
  Input(std::shared_ptr<Graphic> graphic_);
  bool GetKey(uint8_t num);
  MessageType ProcessInput();

 private:
  std::array<bool, 16> key_;

  bool space_is_released_;
  std::shared_ptr<Graphic> graphic_;
};