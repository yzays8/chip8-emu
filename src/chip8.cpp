#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <memory>
#include <chrono>
#include <random>
#include <SDL2/SDL.h>

#include "chip8.hpp"
#include "graphic.hpp"
#include "delay_timer.hpp"
#include "sound_timer.hpp"

Chip8::Chip8(const bool flag_debug)
    : mem_{}, stack_{}, v_{}, i_{0}, pc_{0x200}, sp_{0}, dt_{0}, st_{0},
      flag_debug_{flag_debug}, drawable_{false}, is_sleeping_{false}, is_running_{false},
      gen_{rd_()}, dis_{0, 255} {
  graphic_ = std::make_shared<Graphic>();
  sound_ = std::make_shared<Sound>();
  delay_timer_ = std::make_unique<DelayTimer>(dt_, is_sleeping_);
  sound_timer_ = std::make_unique<SoundTimer>(st_, sound_, is_sleeping_);
  input_ = std::make_unique<Input>(graphic_);
  std::copy(kSprites, kSprites + 80, mem_.begin());
}

void Chip8::LoadROM(const std::string rom) {
  std::ifstream ifs(rom, std::ios::binary | std::ios::in);
  if (!ifs.is_open()) {
    std::cerr << "Failed to open ROM" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  char data;
  for (int i = 0x200; ifs.get(data); ++i) {
    mem_[i] = static_cast<uint8_t>(data);
  }
  std::cout << "Loaded ROM" << std::endl;
}

void Chip8::InitializeWindow(const int window_scale) {
  graphic_->InitializeWindow(window_scale);
}

void Chip8::InitializeSound() {
  sound_->InitializeSound();
  sound_->OpenAudioFile(kBeepFilePath);
}

void Chip8::StartTimers() {
  delay_timer_->Start();
  sound_timer_->Start();
}

void Chip8::Shutdown() {
  std::cout << "Shutdown..." << std::endl;
  drawable_ = false;
  is_sleeping_ = true;
  is_running_ = false;
}

void Chip8::ExitByError() {
  sound_->Terminate();
  sound_timer_->Terminate();
  delay_timer_->Terminate();
  graphic_->Terminate();
  std::exit(EXIT_FAILURE);
}

void Chip8::Tick() {
  uint16_t inst = mem_[pc_] << 8 | mem_[pc_ + 1];
  InterpretInstruction(inst);
}

void Chip8::RunLoop() {
  const auto interval = std::chrono::duration<int, std::ratio<1, kMainCycles>>(1);  // 1/kMainCycles s
  MessageType msg;

  InitializeSound();
  StartTimers();
  is_running_ = true;
  while (is_running_) {
    auto start_time = std::chrono::high_resolution_clock::now();
    msg = input_->ProcessInput(is_sleeping_);
    if (msg == MSG_SHUTDOWN) {
      Shutdown();
    }
    if (!is_sleeping_) {
      Tick();
      if (drawable_) {
        drawable_ = false;
        graphic_->Render();
      }
    }
    std::this_thread::sleep_until(start_time + interval);
  }
}

void Chip8::Debug(const uint16_t inst) {
  printf("Debug: pc=0x%04X, inst=0x%04X, i=0x%04X, sp=0x%02X, dt=0x%02X, st=0x%02X\n",
    pc_, inst, i_, sp_, delay_timer_->GetRegisterValue(), sound_timer_->GetRegisterValue());
}

void Chip8::InterpretInstruction(const uint16_t inst) {
  if (flag_debug_) Debug(inst);

  switch (inst & 0xF000) {
    case 0x0000:
      switch (inst) {
        case 0x00E0:
          graphic_->GetBuffer().fill({}); // 0-fill
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
          ExitByError();
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
          ExitByError();
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
      v_[(inst & 0x0F00) >> 8] = dis_(gen_) & (inst & 0x00FF);
      pc_ += 2;
      break;
    case 0xD000:
      // 0xDxyn
      {
        uint16_t x = v_[(inst & 0x0F00) >> 8] % 64;
        uint16_t y = v_[(inst & 0x00F0) >> 4] % 32;
        const uint16_t n = inst & 0x000F;
        auto& frame_buffer = graphic_->GetBuffer();

        v_[0xF] = 0;
        for (uint16_t h = 0; h < n; ++h) {
          uint8_t sprite = mem_[i_ + h];
          for (uint16_t w = 0; w < 8; ++w) {
            if (x + w >= 64 || y + h >= 32) break;
            if ((sprite & (0x80 >> w)) != 0) {
              if (frame_buffer[y + h][x + w] == 1) {
                v_[0xF]= 1;
              }
              frame_buffer[y + h][x + w] ^= 1;
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
          if (input_->GetKey(v_[(inst & 0x0F00) >> 8]) == 1) {
            pc_ += 2;
          }
          pc_ += 2;
          break;
        case 0x00A1:
          // 0xExA1
          if (input_->GetKey(v_[(inst & 0x0F00) >> 8]) == 0) {
            pc_ += 2;
          }
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << inst << std::endl;
          ExitByError();
      }
      break;
    case 0xF000:
      switch (inst & 0x00FF) {
        case 0x0007:
          // 0xFx07
          v_[(inst & 0x0F00) >> 8] = delay_timer_->GetRegisterValue();
          pc_ += 2;
          break;
        case 0x000A: {
          // 0xFx0A
          bool key_is_pressed = false;
          for (int i = 0; i < 16; ++i) {
            if (input_->GetKey(i) == 1) {
              key_is_pressed = true;
              v_[(inst & 0x0F00) >> 8] = input_->GetKey(i);
            }
          }
          if (key_is_pressed) {
            pc_ += 2;
          }
          }
          break;
        case 0x0015:
          // 0xFx15
          delay_timer_->SetRegisterValue(v_[(inst & 0x0F00) >> 8]);
          pc_ += 2;
          break;
        case 0x0018:
          // 0xFx18
          sound_timer_->SetRegisterValue(v_[(inst & 0x0F00) >> 8]);
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
        case 0x0055:
          // 0xFx55
          // Original COSMAC VIP implementation for old ROMs
          {
            const uint16_t tmp = (inst & 0x0F00) >> 8;
            for (uint16_t i = 0; i <= tmp; ++i) {   // Forgetting the equal sign causes tons of weird behavior
              mem_[i_ + i] = v_[i];
            }
            i_ += tmp + 1;
            pc_ += 2;
          }
          break;
        case 0x0065:
          // 0xFx65
          {
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
          ExitByError();
      }
      break;
    default:
      std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << inst << std::endl;
      ExitByError();
  }
}