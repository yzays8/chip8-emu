#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <SDL2/SDL.h>

#include "chip8.hpp"

Chip8::Chip8()
    : mem_{}, stack_{}, v_{}, i_{0}, pc_{0x200}, sp_{0}, dt_{0}, st_{0}, main_clock_ticks_{0},
      frame_buffer_{}, key_{}, drawable_{false}, sleep_{false}, window_{}, renderer_{}, window_scale_{15},
      pixel_{} {
  std::copy(kSprites, kSprites + 80, mem_.begin());
  srand((unsigned)time(NULL));
}

void Chip8::LoadROM(const std::string rom) {
  std::ifstream ifs(rom, std::ios::binary | std::ios::in);
  if (!ifs.is_open()) {
    std::cerr << "Cannot open ROM" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  char data;
  for (int i = 0x200; ifs.get(data); ++i) {
    mem_[i] = static_cast<uint8_t>(data);
  }
  std::cout << "Loaded ROM" << std::endl;
}

void Chip8::InitializeWindow(const int window_scale) {
  window_scale_ = window_scale;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Cannot initialize SDL" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  if (SDL_CreateWindowAndRenderer(64 * window_scale_, 32 * window_scale_, 0, &window_, &renderer_) != 0) {
    std::cerr << "Cannot create window and renderer" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  SDL_SetWindowTitle(window_, "Chip-8 Emulator");
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
  SDL_RenderClear(renderer_);
  SDL_RenderPresent(renderer_);
  pixel_ = {
    0,              // x (temporary)
    0,              // y (temporary)
    window_scale_,  // width
    window_scale_,  // height
  };

  std::cout << "Initialized window" << std::endl;
}

void Chip8::ProcessInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        std::cout << "Shutdown..." << std::endl;
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
        std::exit(EXIT_SUCCESS);
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
          case SDLK_9:
            sleep_ = true;
            break;
          case SDLK_0:
            sleep_ = false;
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
        }
        break;
    }
  }
}

void Chip8::Tick() {
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), main_clock_ticks_ + 2));  // 500Hz

  uint16_t inst = mem_[pc_] << 8 | mem_[pc_ + 1];
  InterpretInstruction(inst);
  if (dt_ > 0) --dt_;
  if (st_ > 0) {
    printf("\a"); // sound
    --st_;
  }

  main_clock_ticks_ = SDL_GetTicks();
}

void Chip8::Render() {
  drawable_ = false;
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 64; ++j) {
      pixel_.x = window_scale_ * j;
      pixel_.y = window_scale_ * i;
      if (frame_buffer_[i][j] == 1) {
        SDL_SetRenderDrawColor(renderer_, 0, 255, 255, 255);  // cyan
      } else {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);      // black
      }
      SDL_RenderFillRect(renderer_, &pixel_);
    }
  }
  SDL_RenderPresent(renderer_);   // This function should not be placed in the loop
}

void Chip8::RunLoop() {
  for (;;) {
    ProcessInput();
    if (!sleep_) {
      Tick();
      if (drawable_) {
        Render();
      }
    }
  }
}

void Chip8::Debug(const uint16_t inst) {
  printf("Debug Mode: pc=0x%X, inst=0x%04X, i=0x%X, sp=0x%X, dt=0x%X, st=0x%X\n"
  , pc_, inst, i_, sp_, dt_, st_);
}

void Chip8::InterpretInstruction(const uint16_t inst) {
  // Debug(inst);
  switch (inst & 0xF000) {
    case 0x0000:
      switch (inst) {
        case 0x00E0:
          frame_buffer_.fill({}); // 0-fill
          drawable_ = true;
          pc_ += 2;
          break;
        case 0x00EE:
          --sp_;
          pc_ = stack_[sp_]; // pop
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << inst << std::endl;
          std::exit(EXIT_FAILURE);
      }
      break;
    case 0x1000:
      // 0x1nnn
      pc_ = inst & 0x0FFF;
      break;
    case 0x2000:
      // 0x2nnn
      stack_[sp_] = pc_;  // push
      ++sp_;
      pc_ = inst & 0x0FFF;
      break;
    case 0x3000:
      // 0x3xkk
      if (v_[(inst & 0x0F00) >> 8] == (inst & 0x00FF)) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0x4000:
      // 0x4xkk
      if (v_[(inst & 0x0F00) >> 8] != (inst & 0x00FF)) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0x5000:
      // 0x5xy0
      if (v_[(inst & 0x0F00) >> 8] == v_[(inst & 0x00F0) >> 4]) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0x6000:
      // 0x6xkk
      v_[(inst & 0x0F00) >> 8] = inst & 0x00FF;
      pc_ += 2;
      break;
    case 0x7000:
      // 0x7xkk
      v_[(inst & 0x0F00) >> 8] += inst & 0x00FF;
      pc_ += 2;
      break;
    case 0x8000:
      switch (inst & 0x000F) {
        case 0x0000:
          // 0x8xy0
          v_[(inst & 0x0F00) >> 8] = v_[(inst & 0x00F0) >> 4];
          pc_ += 2;
          break;
        case 0x0001:
          // 0x8xy1
          v_[(inst & 0x0F00) >> 8] |= v_[(inst & 0x00F0) >> 4];
          v_[0xF] = 0;
          pc_ += 2;
          break;
        case 0x0002:
          // 0x8xy2
          v_[(inst & 0x0F00) >> 8] &= v_[(inst & 0x00F0) >> 4];
          v_[0xF] = 0;
          pc_ += 2;
          break;
        case 0x0003:
          // 0x8xy3
          v_[(inst & 0x0F00) >> 8] ^= v_[(inst & 0x00F0) >> 4];
          v_[0xF] = 0;
          pc_ += 2;
          break;
        case 0x0004: {
          // 0x8xy4
          uint16_t sum = v_[(inst & 0x0F00) >> 8] + v_[(inst & 0x00F0) >> 4];
          if (sum > 0xFF) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          v_[(inst & 0x0F00) >> 8] = sum & 0xFF;  // fix the overflow
          pc_ += 2;
          }
          break;
        case 0x0005:
          // 0x8xy5
          if (v_[(inst & 0x0F00) >> 8] > v_[(inst & 0x00F0) >> 4]) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          v_[(inst & 0x0F00) >> 8] -= v_[(inst & 0x00F0) >> 4];
          pc_ += 2;
          break;
        case 0x0006:
          // 0x8xy6
          v_[0xF] = v_[(inst & 0x0F00) >> 8] & 0x01;
          v_[(inst & 0x0F00) >> 8] >>= 1;
          pc_ += 2;
          break;
        case 0x0007:
          // 0x8xy7
          if (v_[(inst & 0x00F0) >> 4] > v_[(inst & 0x0F00) >> 8]) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          v_[(inst & 0x0F00) >> 8] = v_[(inst & 0x00F0) >> 4] - v_[(inst & 0x0F00) >> 8];
          pc_ += 2;
          break;
        case 0x000E:
          // 0x8xyE
          v_[0xF] = v_[(inst & 0x0F00) >> 8] >> 7;
          v_[(inst & 0x0F00) >> 8] <<= 1;
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << inst << std::endl;
          std::exit(EXIT_FAILURE);
      }
      break;
    case 0x9000:
      // 0x9xy0
      if (v_[(inst & 0x0F00) >> 8] != v_[(inst & 0x00F0) >> 4]) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0xA000:
      // 0xAnnn
      i_ = inst & 0x0FFF;
      pc_ += 2;
      break;
    case 0xB000:
      // 0xBnnn
      pc_ = (inst & 0x0FFF) + v_[0];
      break;
    case 0xC000:
      // 0xCxkk
      v_[(inst & 0x0F00) >> 8] = (rand() % 256) & (inst & 0x00FF);
      pc_ += 2;
      break;
    case 0xD000: {
      // 0xDxyn
      uint16_t x = v_[(inst & 0x0F00) >> 8] % 64;
      uint16_t y = v_[(inst & 0x00F0) >> 4] % 32;
      const uint16_t n = inst & 0x000F;

      v_[0xF] = 0;
      for (int h = 0; h < n; ++h) {
        uint8_t sprite = mem_[i_ + h];
        for (int w = 0; w < 8; ++w) {
          if (x + w >= 64 || y + h >= 32) break;
          if ((sprite & (0x80 >> w)) != 0) {
            if (frame_buffer_[y + h][x + w] == 1) {
              v_[0xF]= 1;
            }
            frame_buffer_[y + h][x + w] ^= 1;
          }
        }
      }

      drawable_ = true;
      pc_ += 2;
      }
      break;
    case 0xE000:
      switch (inst & 0x00FF) {
        case 0x009E:
          // 0xEx9E
          if (key_[v_[(inst & 0x0F00) >> 8]] == 1) {
            pc_ += 2;
          }
          pc_ += 2;
          break;
        case 0x00A1:
          // 0xExA1
          if (key_[v_[(inst & 0x0F00) >> 8]] == 0) {
            pc_ += 2;
          }
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << inst << std::endl;
          std::exit(EXIT_FAILURE);
      }
      break;
    case 0xF000:
      switch (inst & 0x00FF) {
        case 0x0007:
          // 0xFx07
          v_[(inst & 0x0F00) >> 8] = dt_;
          pc_ += 2;
          break;
        case 0x000A: {
          // 0xFx0A
          bool key_is_pressed = false;
          for (int i = 0; i < 16; ++i) {
            if (key_[i] == 1) {
              key_is_pressed = true;
              v_[(inst & 0x0F00) >> 8] = key_[i];
            }
          }
          if (key_is_pressed) {
            pc_ += 2;
          }
          }
          break;
        case 0x0015:
          // 0xFx15
          dt_ = v_[(inst & 0x0F00) >> 8];
          pc_ += 2;
          break;
        case 0x0018:
          // 0xFx18
          st_ = v_[(inst & 0x0F00) >> 8];
          pc_ += 2;
          break;
        case 0x001E:
          // 0xFx1E
          // Amiga
          if ((i_ += v_[(inst & 0x0F00) >> 8]) > 0x0FFF) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          pc_ += 2;
          break;
        case 0x0029:
          // 0xFx29
          i_ = 5 * v_[(inst & 0x0F00) >> 8]; // The first address of the sprites is 0x000
          pc_ += 2;
          break;
        case 0x0033:
          // 0xFx33
          mem_[i_] = v_[(inst & 0x0F00) >> 8] / 100;
          mem_[i_ + 1] = (v_[(inst & 0x0F00) >> 8] / 10) % 10;
          mem_[i_ + 2] = v_[(inst & 0x0F00) >> 8] % 10;
          pc_ += 2;
          break;
        case 0x0055: {
          // 0xFx55
          // Original COSMAC VIP implementation for old ROMs
          const uint16_t tmp = (inst & 0x0F00) >> 8;
          for (uint16_t i = 0; i <= tmp; ++i) {   // Forgetting the equal sign causes tons of weird behavior
            mem_[i_ + i] = v_[i];
          }
          i_ += tmp + 1;
          pc_ += 2;
          }
          break;
        case 0x0065: {
          // 0xFx65
          const uint16_t tmp = (inst & 0x0F00) >> 8;
          for (uint16_t i = 0; i <= tmp; ++i) {   // Forgetting the equal sign causes tons of weird behavior
            v_[i] = mem_[i_ + i];
          }
          i_ += tmp + 1;
          pc_ += 2;
          }
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << inst << std::endl;
          std::exit(EXIT_FAILURE);
      }
      break;
    default:
      std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << inst << std::endl;
      std::exit(EXIT_FAILURE);
  }
}