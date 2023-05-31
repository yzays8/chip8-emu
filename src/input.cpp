#include <ctime>
#include <SDL2/SDL.h>

#include "input.hpp"
#include "graphic.hpp"

namespace chip8_emu {

Input::Input(std::shared_ptr<Graphic> graphic)
    : key_{}, space_is_released_{true}, graphic_{graphic} {
  std::srand((unsigned)time(nullptr));
}

bool Input::GetKey(uint8_t num) {
  return key_[num];
}

MessageType Input::ProcessInput() {
  SDL_Event event;
  MessageType msg = MSG_NONE;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        msg = MSG_SHUTDOWN;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_1:
            key_[0x1] = 1;
            break;
          case SDLK_2:
            key_[0x2] = 1;
            break;
          case SDLK_3:
            key_[0x3] = 1;
            break;
          case SDLK_4:
            key_[0xC] = 1;
            break;
          case SDLK_q:
            key_[0x4] = 1;
            break;
          case SDLK_w:
            key_[0x5] = 1;
            break;
          case SDLK_e:
            key_[0x6] = 1;
            break;
          case SDLK_r:
            key_[0xD] = 1;
            break;
          case SDLK_a:
            key_[0x7] = 1;
            break;
          case SDLK_s:
            key_[0x8] = 1;
            break;
          case SDLK_d:
            key_[0x9] = 1;
            break;
          case SDLK_f:
            key_[0xE] = 1;
            break;
          case SDLK_z:
            key_[0xA] = 1;
            break;
          case SDLK_x:
            key_[0x0] = 1;
            break;
          case SDLK_c:
            key_[0xB] = 1;
            break;
          case SDLK_v:
            key_[0xF] = 1;
            break;
          case SDLK_ESCAPE:
            msg = MSG_SHUTDOWN;
            break;
          case SDLK_SPACE:
            // allow only one push
            if (space_is_released_) {
              msg = MSG_CHANGE_SLEEP_STATE;
              space_is_released_ = false;
            }
            break;
          case SDLK_t:
            msg = MSG_TICK_WHILE_SLEEP;
            break;
          case SDLK_9: {
            Color color {
              static_cast<uint8_t>(rand() % 256),
              static_cast<uint8_t>(rand() % 256),
              static_cast<uint8_t>(rand() % 256),
            };
            graphic_->ChangeObjectColor(color);
            msg = MSG_REDRAW;
            break;
          }
          case SDLK_0: {
            Color color {
              static_cast<uint8_t>(rand() % 256),
              static_cast<uint8_t>(rand() % 256),
              static_cast<uint8_t>(rand() % 256),
            };
            graphic_->ChangeBackGroundColor(color);
            msg = MSG_REDRAW;
            break;
          }
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
          case SDLK_1:
            key_[0x1] = 0;
            break;
          case SDLK_2:
            key_[0x2] = 0;
            break;
          case SDLK_3:
            key_[0x3] = 0;
            break;
          case SDLK_4:
            key_[0xC] = 0;
            break;
          case SDLK_q:
            key_[0x4] = 0;
            break;
          case SDLK_w:
            key_[0x5] = 0;
            break;
          case SDLK_e:
            key_[0x6] = 0;
            break;
          case SDLK_r:
            key_[0xD] = 0;
            break;
          case SDLK_a:
            key_[0x7] = 0;
            break;
          case SDLK_s:
            key_[0x8] = 0;
            break;
          case SDLK_d:
            key_[0x9] = 0;
            break;
          case SDLK_f:
            key_[0xE] = 0;
            break;
          case SDLK_z:
            key_[0xA] = 0;
            break;
          case SDLK_x:
            key_[0x0] = 0;
            break;
          case SDLK_c:
            key_[0xB] = 0;
            break;
          case SDLK_v:
            key_[0xF] = 0;
            break;
          case SDLK_SPACE:
            space_is_released_ = true;
            break;
        }
        break;
    }
  }
  return msg;
}

}  // namespace chip8_emu
