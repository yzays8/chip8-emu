#pragma once

#include <array>
#include <atomic>
#include <memory>

#include "graphic.hpp"

enum MessageType {
  MSG_NONE,
  MSG_SHUTDOWN
};

class Input {
 public:
  Input(std::shared_ptr<Graphic> graphic_);
  bool GetKey(uint8_t num);
  MessageType ProcessInput(std::atomic_bool& is_sleeping);

 private:
  std::array<bool, 16> key_;

  bool space_is_released_;
  std::shared_ptr<Graphic> graphic_;
};