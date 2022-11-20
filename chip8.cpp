#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <SDL2/SDL.h>

#include "chip8.hpp"

Chip8::Chip8()
    : frameBuffer{}, key{}, mem_{}, stack_{}, v_{}, i_{0},
      pc_{0x200}, sp_{0}, dt_{0}, st_{0} {
  const uint8_t sprites[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
  };
  memcpy(mem_, sprites, 80);

  srand((unsigned)time(NULL));
}

void Chip8::LoadROM(const std::string rom) {
  std::ifstream ifs(rom, std::ios::binary | std::ios::in);
  if (!ifs.is_open()) exit(1);
  char data;
  for (int i = 0x200; ifs.get(data); ++i) {
    mem_[i] = (uint8_t)data;
  }
  std::cout << "Loaded ROM!" << std::endl;
}

void Chip8::InterpretInstruction(const uint16_t ins) {
  //printf("Instruction tracking: pc_=%X, op=0x%04X, i_=%X, sp_=%X\n", pc_, ins, i_, sp_);
  switch (ins & 0xF000) {
    case 0x0000:
      switch (ins) {
        case 0x00E0:
          frameBuffer.fill({}); // 0-fill
          drawable = true;
          pc_ += 2;
          break;
        case 0x00EE:
          --sp_;
          pc_ = stack_[sp_]; // pop
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    case 0x1000:
      // 0x1nnn
      pc_ = ins & 0x0FFF;
      break;
    case 0x2000:
      // 0x2nnn
      stack_[sp_] = pc_;  // push
      ++sp_;
      pc_ = ins & 0x0FFF;
      break;
    case 0x3000:
      // 0x3xkk
      if (v_[(ins & 0x0F00) >> 8] == (ins & 0x00FF)) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0x4000:
      // 0x4xkk
      if (v_[(ins & 0x0F00) >> 8] != (ins & 0x00FF)) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0x5000:
      // 0x5xy0
      if (v_[(ins & 0x0F00) >> 8] == v_[(ins & 0x00F0) >> 4]) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0x6000:
      // 0x6xkk
      v_[(ins & 0x0F00) >> 8] = ins & 0x00FF;
      pc_ += 2;
      break;
    case 0x7000:
      // 0x7xkk
      v_[(ins & 0x0F00) >> 8] += ins & 0x00FF;
      pc_ += 2;
      break;
    case 0x8000:
      switch (ins & 0x000F) {
        case 0x0000:
          // 0x8xy0
          v_[(ins & 0x0F00) >> 8] = v_[(ins & 0x00F0) >> 4];
          pc_ += 2;
          break;
        case 0x0001:
          // 0x8xy1
          v_[(ins & 0x0F00) >> 8] |= v_[(ins & 0x00F0) >> 4];
          v_[0xF] = 0;
          pc_ += 2;
          break;
        case 0x0002:
          // 0x8xy2
          v_[(ins & 0x0F00) >> 8] &= v_[(ins & 0x00F0) >> 4];
          v_[0xF] = 0;
          pc_ += 2;
          break;
        case 0x0003:
          // 0x8xy3
          v_[(ins & 0x0F00) >> 8] ^= v_[(ins & 0x00F0) >> 4];
          v_[0xF] = 0;
          pc_ += 2;
          break;
        case 0x0004: {
          // 0x8xy4
          uint16_t sum = v_[(ins & 0x0F00) >> 8] + v_[(ins & 0x00F0) >> 4];
          if (sum > 0xFF) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          v_[(ins & 0x0F00) >> 8] = sum & 0xFF;  // fix the overflow
          }
          pc_ += 2;
          break;
        case 0x0005:
          // 0x8xy5
          if (v_[(ins & 0x0F00) >> 8] > v_[(ins & 0x00F0) >> 4]) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          v_[(ins & 0x0F00) >> 8] -= v_[(ins & 0x00F0) >> 4];
          pc_ += 2;
          break;
        case 0x0006:
          // 0x8xy6
          v_[0xF] = v_[(ins & 0x0F00) >> 8] & 0x01;
          v_[(ins & 0x0F00) >> 8] >>= 1;
          pc_ += 2;
          break;
        case 0x0007:
          // 0x8xy7
          if (v_[(ins & 0x00F0) >> 4] > v_[(ins & 0x0F00) >> 8]) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          v_[(ins & 0x0F00) >> 8] = v_[(ins & 0x00F0) >> 4] - v_[(ins & 0x0F00) >> 8];
          pc_ += 2;
          break;
        case 0x000E:
          // 0x8xyE
          v_[0xF] = v_[(ins & 0x0F00) >> 8] >> 7;
          v_[(ins & 0x0F00) >> 8] <<= 1;
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    case 0x9000:
      // 0x9xy0
      if (v_[(ins & 0x0F00) >> 8] != v_[(ins & 0x00F0) >> 4]) {
        pc_ += 2;
      }
      pc_ += 2;
      break;
    case 0xA000:
      // 0xAnnn
      i_ = ins & 0x0FFF;
      pc_ += 2;
      break;
    case 0xB000:
      // 0xBnnn
      pc_ = (ins & 0x0FFF) + v_[0];
      break;
    case 0xC000:
      // 0xCxkk
      v_[(ins & 0x0F00) >> 8] = (rand() % 256) & (ins & 0x00FF);
      pc_ += 2;
      break;
    case 0xD000: {
      // 0xDxyn
      uint16_t x = v_[(ins & 0x0F00) >> 8] % 64;
      uint16_t y = v_[(ins & 0x00F0) >> 4] % 32;
      const uint16_t n = ins & 0x000F;

      v_[0xF] = 0;
      for (int h = 0; h < n; ++h) {
        uint8_t sprite = mem_[i_ + h];
        for (int w = 0; w < 8; ++w) {
          if (x + w >= 64 || y + h >= 32) break;
          if ((sprite & (0x80 >> w)) != 0) {
            if (frameBuffer[y + h][x + w] == 1) {
              v_[0xF]= 1;
            }
            frameBuffer[y + h][x + w] ^= 1;
          }
        }
      }

      }
      drawable = true;
      pc_ += 2;
      break;
    case 0xE000:
      switch (ins & 0x00FF) {
        case 0x009E:
          // 0xEx9E
          if (key[v_[(ins & 0x0F00) >> 8]] == 1) {
            pc_ += 2;
          }
          pc_ += 2;
          break;
        case 0x00A1:
          // 0xExA1
          if (key[v_[(ins & 0x0F00) >> 8]] == 0) {
            pc_ += 2;
          }
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    case 0xF000:
      switch (ins & 0x00FF) {
        case 0x0007:
          // 0xFx07
          v_[(ins & 0x0F00) >> 8] = dt_;
          pc_ += 2;
          break;
        case 0x000A: {
          // 0xFx0A
          bool keyIsPressed = false;
          for (int i = 0; i < 16; ++i) {
            if (key[i] == 1) {
              keyIsPressed = true;
              v_[(ins & 0x0F00) >> 8] = key[i];
            }
          }
          if (keyIsPressed) {
            pc_ += 2;
          }
        } break;
        case 0x0015:
          // 0xFx15
          dt_ = v_[(ins & 0x0F00) >> 8];
          pc_ += 2;
          break;
        case 0x0018:
          // 0xFx18
          st_ = v_[(ins & 0x0F00) >> 8];
          pc_ += 2;
          break;
        case 0x001E:
          // 0xFx1E
          // Amiga
          if ((i_ += v_[(ins & 0x0F00) >> 8]) > 0x0FFF) {
            v_[0xF] = 1;
          } else {
            v_[0xF] = 0;
          }
          pc_ += 2;
          break;
        case 0x0029:
          // 0xFx29
          i_ = 5 * v_[(ins & 0x0F00) >> 8]; // The first address of the sprites is 0x000
          pc_ += 2;
          break;
        case 0x0033:
          // 0xFx33
          mem_[i_] = v_[(ins & 0x0F00) >> 8] / 100;
          mem_[i_ + 1] = (v_[(ins & 0x0F00) >> 8] / 10) % 10;
          mem_[i_ + 2] = v_[(ins & 0x0F00) >> 8] % 10;
          pc_ += 2;
          break;
        case 0x0055: {
          // 0xFx55
          // Original COSMAC VIP implementation for old ROMs
          const uint16_t tmp = (ins & 0x0F00) >> 8;
          for (uint16_t i = 0; i <= tmp; ++i) {   // If you forget the equal sign, it causes countless bugs!
            mem_[i_ + i] = v_[i];
          }
          i_ += tmp + 1;
          }
          pc_ += 2;
          break;
        case 0x0065: {
          // 0xFx65
          const uint16_t tmp = (ins & 0x0F00) >> 8;
          for (uint16_t i = 0; i <= tmp; ++i) {   // If you forget the equal sign, it causes countless bugs!
            v_[i] = mem_[i_ + i];
          }
          i_ += tmp + 1;
          }
          pc_ += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    default:
      std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
      exit(1);
  }
}

void Chip8::RunLoop() {
  uint16_t op = mem_[pc_] << 8 | mem_[pc_ + 1];
  InterpretInstruction(op);
  if (dt_ > 0) --dt_;
  if (st_ > 0) {
    printf("\a"); // sound
    --st_;
  }
}