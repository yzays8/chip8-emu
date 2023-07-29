#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include "utils.hpp"
#include "graphic.hpp"

namespace chip8_emu {

enum MessageType {
  MSG_NONE,
  MSG_CHANGE_SLEEP_STATE,
  MSG_TICK_WHILE_SLEEP,
  MSG_REDRAW,
  MSG_SHUTDOWN,
};

class Input {
 public:
  Input(std::shared_ptr<Graphic> graphic_);
  bool GetKey(uint8_t num) const;
  MessageType ProcessInput();

 private:
  std::array<bool, 16> key_;

  bool space_is_released_;
  std::unique_ptr<Rand> rand_;
  std::shared_ptr<Graphic> graphic_;
};

} // namespace chip8_emu
