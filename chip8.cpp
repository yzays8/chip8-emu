#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <SDL2/SDL.h>
#include "chip8.h"

Chip8::Chip8() {
  pc = 0x200;
  i = 0;
  sp = 0;
  dt = 0;
  st = 0;
  isDrawn = false;
  for (int i = 0; i < 16; i++) {
    v[i] = 0;
  }
  for (int i = 0; i < 16; i++) {
    stack[i] = 0;
  }
  for (int i = 0; i < 4096; i++) {
    mem[i] = 0;
  }
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 64; j++) {
      frameBuf[i][j] = 0;
    }
  }
  for (int i = 0; i < 16; i++) {
    key[i] = 0;
  }

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
  for (int i = 0; i < 80; i++) {
    mem[i] = sprites[i];
  }

  srand((unsigned)time(NULL));
}

void Chip8::loadROM(const std::string rom) {
  std::ifstream ifs(rom, std::ios::binary | std::ios::in);
  if (!ifs.is_open()) exit(1);
  char data;
  for (int i = 0x200; ifs.get(data); i++) {
    mem[i] = (uint8_t)data;
  }
  std::cout << "Loaded ROM" << std::endl;
}

void Chip8::interpretInstruction(const uint16_t ins) {
  //printf("Instruction tracking: 0x%X\n", ins);
  switch (ins & 0xF000) {
    case 0x0000:
      switch (ins) {
        case 0x00E0:
          for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 64; j++) {
              frameBuf[i][j] = 0;
            }
          }
          isDrawn = true;
          pc += 2;
          break;
        case 0x00EE:
          pc = stack[--sp]; // pop
          pc += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    case 0x1000:
      // 0x1nnn
      pc = ins & 0x0FFF;
      break;
    case 0x2000:
      // 0x2nnn, push
      stack[sp++] = pc;
      pc = ins & 0x0FFF;
      break;
    case 0x3000:
      // 0x3xkk
      if (v[(ins & 0x0F00) >> 8] == (ins & 0x00FF)) {
        pc += 2;
      }
      pc += 2;
      break;
    case 0x4000:
      // 0x4xkk
      if (v[(ins & 0x0F00) >> 8] != (ins & 0x00FF)) {
        pc += 2;
      }
      pc += 2;
      break;
    case 0x5000:
      // 0x5xy0
      if (v[(ins & 0x0F00) >> 8] == v[(ins & 0x00F0) >> 4]) {
        pc += 2;
      }
      pc += 2;
      break;
    case 0x6000:
      // 0x6xkk
      v[(ins & 0x0F00) >> 8] = ins & 0x00FF;
      pc += 2;
      break;
    case 0x7000:
      // 0x7xkk
      v[(ins & 0x0F00) >> 8] += ins & 0x00FF;
      pc += 2;
      break;
    case 0x8000:
      switch (ins & 0x000F) {
        case 0x0000:
          // 0x8xy0
          v[(ins & 0x0F00) >> 8] = v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0001:
          // 0x8xy1
          v[(ins & 0x0F00) >> 8] |= v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0002:
          // 0x8xy2
          v[(ins & 0x0F00) >> 8] &= v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0003:
          // 0x8xy3
          v[(ins & 0x0F00) >> 8] ^= v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        case 0x0004:
          // 0x8xy4
          if ((v[(ins & 0x0F00) >> 8] += v[(ins & 0x00F0) >> 4]) > 0xFF) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          pc += 2;
          break;
        case 0x0005:
          // 0x8xy5
          if ((v[(ins & 0x0F00) >> 8] -= v[(ins & 0x00F0) >> 4]) > 0) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          pc += 2;
          break;
        case 0x0006:
          // 0x8xy6
          if ((v[(ins & 0x0F00) >> 8] & 0x1) == 1) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          v[(ins & 0x0F00) >> 8] >>= 1;
          pc += 2;
          break;
        case 0x0007:
          // 0x8xy7
          if (v[(ins & 0x00F0) >> 4] > v[(ins & 0x0F00) >> 8]) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          v[(ins & 0x0F00) >> 8] = v[(ins & 0x00F0) >> 4] - v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x000E:
          // 0x8xyE
          if (v[(ins & 0x0F00) >> 8] & 0x80 == 0x80) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          v[(ins & 0x0F00) >> 8] <<= 1;
          pc += 2;
          break;
        default:
          std::cerr << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    case 0x9000:
      // 0x9xy0
      if (v[(ins & 0x0F00) >> 8] != v[(ins & 0x00F0) >> 4]) {
        pc += 2;
      }
      pc += 2;
      break;
    case 0xA000:
      // 0xAnnn
      i = ins & 0x0FFF;
      pc += 2;
      break;
    case 0xB000:
      // 0xBnnn
      pc = (ins & 0x0FFF) + v[0];
      break;
    case 0xC000:
      // 0xCxkk
      v[(ins & 0x0F00) >> 8] = (rand() % 256) & (ins & 0x00FF);
      pc += 2;
      break;
    case 0xD000: {
      // 0xDxyn
      uint16_t x = v[(ins & 0x0F00) >> 8] % 64;
      uint16_t y = v[(ins & 0x00F0) >> 4] % 32;
      uint16_t n = ins & 0x000F;

      for (int h = 0; h < n; h++) {
        for (int w = 0; w < 8; w++) {
          if (x + w < 64 && y + h < 32) {
            uint8_t pixel = mem[i + h] & (0x80 >> w);
            if (pixel != 0) {
              if (frameBuf[y + h][x + w] == 1) {
                v[0xF]= 1;
              } else {
                v[0xF] = 0;
              }
              frameBuf[y + h][x + w] ^= 1;
            }
          }
        }
      }
      }
      isDrawn = true;
      pc += 2;
      break;
    case 0xE000:
      switch (ins & 0x00FF) {
        case 0x009E:
          // 0xEx9E
          if (key[v[(ins & 0x0F00) >> 8]] == 1) {
            pc += 2;
          }
          pc += 2;
          break;
        case 0x00A1:
          // 0xExA1
          if (key[v[(ins & 0x0F00) >> 8]] == 0) {
            pc += 2;
          }
          pc += 2;
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
          v[(ins & 0x0F00) >> 8] = dt;
          pc += 2;
          break;
        case 0x000A: {
          // 0xFx0A
          bool keyIsPressed = false;
          for (int i = 0; i < 16; i++) {
            if (key[i] == 1) {
              keyIsPressed = true;
              v[(ins & 0x0F00) >> 8] = key[i];
            }
          }
          if (keyIsPressed) {
            pc += 2;
          }
        } break;
        case 0x0015:
          // 0xFx15
          dt = v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x0018:
          // 0xFx18
          st = v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x001E:
          // 0xFx1E
          // Original COSMAC VIP implementation
          i += v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        case 0x0029:
          // 0xFx29
          i = 5 * v[(ins & 0x0F00) >> 8]; // The first address of the sprites is 0x000
          pc += 2;
          break;
        case 0x0033:
          // 0xFx33
          mem[i] = v[(ins & 0x0F00) >> 8] / 100;
          mem[i + 1] = v[(ins & 0x0F00) >> 8] / 10 % 10;
          mem[i + 2] = v[(ins & 0x0F00) >> 8] % 10;
          pc += 2;
          break;
        case 0x0055: {
          // 0xFx55
          // Modern implementation
          int tmp = (ins & 0x0F00) >> 8;
          for (int j = 0; j < tmp; j++) {
            mem[i + j] = v[j];
          }
          }
          pc += 2;
          break;
        case 0x0065: {
          // 0xFx65
          int tmp = (ins & 0x0F00) >> 8;
          for (int j = 0; j < tmp; j++) {
            v[j] = mem[i + j];
          }
          }
          pc += 2;
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

void Chip8::runLoop() {
  interpretInstruction(mem[pc] << 8 | mem[pc+1]);
  if (dt > 0) dt--;
  if (st > 0) {
    st--;
    // sound
  }
}

Chip8::~Chip8(){}