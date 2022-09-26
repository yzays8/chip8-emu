#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include "fontset.h"
#include <SDL2/SDL.h>

const int MEM_SIZE = 4096;

class Chip8 {
 public:
  Chip8() {
    pc = 0x200;
    i = 0;
    sp = 0;
    dt = 0;
    st = 0;
    isDrew = false;
    for (int i = 0; i < sizeof(v)/sizeof(v[0]); i++) {
      v[i] = 0;
    }
    for (int i = 0; i < sizeof(stack)/sizeof(stack[0]); i++) {
      stack[i] = 0;
    }
    for (int i = 0; i < MEM_SIZE; i++) {
      mem[i] = 0;
    }
    for (int i = 0; i < 32; i++) {
      for (int j = 0; j < 64; j++) {
        frameBuf[i][j] = 0;
      }
    }
    for (int i = 0; i < sizeof(sprites)/sizeof(sprites[0]); i++) {
      mem[i] = sprites[i];
    }
    for (int i = 0; i < 16; i++) {
      key[i] = 0;
    }
    srand((unsigned)time(NULL));
  }

  uint8_t mem[MEM_SIZE];
  int frameBuf[32][64];  // 0:black, 1:white
  int key[16];
  bool isDrew;
  void loadROM(const std::string rom);
  void interpretInstruction(const uint16_t ins);
  void runLoop();
  ~Chip8(){};

  private:
   uint8_t v[16];
   uint16_t i;
   uint16_t pc;
   uint8_t sp;
   uint16_t stack[16];
   uint8_t dt;
   uint8_t st;
};

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
  printf("Instruction tracking: 0x%X\n", ins);
  switch (ins & 0xF000) {
    case 0x0000:
      switch (ins) {
        case 0x00E0:
          // clear display
          for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 64; j++) {
              frameBuf[i][j] = 0;
            }
          }
          isDrew = true;
          pc += 2;
          break;
        case 0x00EE:
          //pc = stack[sp--];
          pc = stack[--sp];
          pc += 2;
          break;
        default:
          std::cout << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    // 0x1nnn
    case 0x1000:
      pc = ins & 0x0FFF;
      break;
    // 0x2nnn
    case 0x2000:
      stack[sp++] = pc;
      pc = ins & 0x0FFF;
      break;
    // 0x3xkk
    case 0x3000:
      if (v[(ins & 0x0F00) >> 8] == (ins & 0x00FF)) {
        pc += 2;
      }
      pc += 2;
      break;
    // 0x4xkk
    case 0x4000:
      if (v[(ins & 0x0F00) >> 8] != (ins & 0x00FF)) {
        pc += 2;
      }
      pc += 2;
      break;
    // 0x5xy0
    case 0x5000:
      if (v[(ins & 0x0F00) >> 8] == v[(ins & 0x00F0) >> 4]) {
        pc += 2;
      }
      pc += 2;
      break;
    // 0x6xkk
    case 0x6000:
      v[(ins & 0x0F00) >> 8] = ins & 0x00FF;
      pc += 2;
      break;
    // 0x7xkk
    case 0x7000:
      v[(ins & 0x0F00) >> 8] += ins & 0x00FF;
      pc += 2;
      break;
    case 0x8000:
      switch (ins & 0x000F) {
        // 0x8xy0
        case 0x0000:
          v[(ins & 0x0F00) >> 8] = v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        // 0x8xy1
        case 0x0001:
          v[(ins & 0x0F00) >> 8] |= v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        // 0x8xy2
        case 0x0002:
          v[(ins & 0x0F00) >> 8] &= v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        // 0x8xy3
        case 0x0003:
          v[(ins & 0x0F00) >> 8] ^= v[(ins & 0x00F0) >> 4];
          pc += 2;
          break;
        // 0x8xy4
        case 0x0004:
          if ((v[(ins & 0x0F00) >> 8] += v[(ins & 0x00F0) >> 4]) > 0xFF) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          pc += 2;
          break;
        // 0x8xy5
        case 0x0005:
          if ((v[(ins & 0x0F00) >> 8] -= v[(ins & 0x00F0) >> 4]) > 0) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          pc += 2;
          break;
        // 0x8xy6
        case 0x0006: //検討
          if ((v[(ins & 0x0F00) >> 8] & 0x01) == 1) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          v[(ins & 0x0F00) >> 8] >>= 1;
          pc += 2;
          break;
        // 0x8xy7
        case 0x0007:
          if (v[(ins & 0x00F0) >> 4] > v[(ins & 0x0F00) >> 8]) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          v[(ins & 0x0F00) >> 8] = v[(ins & 0x00F0) >> 4] - v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        // 0x8xyE
        case 0x000E:
          if (v[(ins & 0x0F00) >> 8] & 0x80 == 1) {
            v[0xF] = 1;
          } else {
            v[0xF] = 0;
          }
          v[(ins & 0x0F00) >> 8] <<= 1;
          pc += 2;
          break;
        default:
          std::cout << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    // 0x9xy0
    case 0x9000:
      if (v[(ins & 0x0F00) >> 8] != v[(ins & 0x00F0) >> 4]) {
        pc += 2;
      }
      pc += 2;
      break;
    // 0xAnnn
    case 0xA000:
      i = ins & 0x0FFF;
      pc += 2;
      break;
    // 0xBnnn
    case 0xB000:
      pc = (ins & 0x0FFF) + v[0];
      break;
    // 0xCxkk
    case 0xC000:
      v[(ins & 0x0F00) >> 8] = (rand() % 256) & (ins & 0x00FF);
      pc += 2;
      break;
    // 0xDxyn
    case 0xD000: {
      // 画面描画
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
      isDrew = true;
      pc += 2;
      break;
    case 0xE000:
      switch (ins & 0x00FF) {
        // 0xEx9E
        case 0x009E:
          // keyboard check
          if (key[v[(ins & 0x0F00) >> 8]] == 1) {
            pc += 2;
          }
          pc += 2;
          break;
        // 0xExA1
        case 0x00A1:
          //keyboard check
          if (key[v[(ins & 0x0F00) >> 8]] == 0) {
            pc += 2;
          }
          pc += 2;
          break;
        default:
          std::cout << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    case 0xF000:
      switch (ins & 0x00FF) {
        // 0xFx07
        case 0x0007:
          v[(ins & 0x0F00) >> 8] = dt;
          pc += 2;
          break;
        // 0xFx0A
        case 0x000A: {
          // keyboard check
          bool keyWasPressed = false;
          for (int i = 0; i < 16; i++) {
            if (key[i] == 1) {
              keyWasPressed = true;
              v[(ins & 0x0F00) >> 8] = key[i];
            }
          }
          if (keyWasPressed) {
            pc += 2;
          }
        } break;
        // 0xFx15
        case 0x0015:
          dt = v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        // 0xFx18
        case 0x0018:
          st = v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        // 0xFx1E
        case 0x001E:
          i += v[(ins & 0x0F00) >> 8];
          pc += 2;
          break;
        // 0xFx29
        case 0x0029:
          i = 5 * v[(ins & 0x0F00) >> 8]; // The first address of the sprites is 0x000
          pc += 2;
          break;
        // 0xFx33
        case 0x0033:
          mem[i] = v[(ins & 0x0F00) >> 8] / 100;
          mem[i+1] = v[(ins & 0x0F00) >> 8] / 10 % 10;
          mem[i+2] = v[(ins & 0x0F00) >> 8] % 10;
          pc += 2;
          break;
        // 0xFx55
        case 0x0055: {
          int tmp = (ins & 0x0F00) >> 8;
          for (int j = 0; j < tmp; j++) {
            mem[i+j] = v[j];
          }
          }
          pc += 2;
          break;
        // 0xFx65
        case 0x0065: {
          int tmp = (ins & 0x0F00) >> 8;
          for (int j = 0; j < tmp; j++) {
            v[j] = mem[i+j];
          }
          }
          pc += 2;
          break;
        default:
          std::cout << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
          exit(1);
      }
      break;
    default:
      std::cout << "Non-existent instruction: 0x" << std::uppercase << std::hex << ins << std::endl;
      exit(1);
  }
}

void Chip8::runLoop() {
  interpretInstruction(mem[pc] << 8 | mem[pc+1]);
  if (dt > 0) dt--;
  if (st > 0) st--;
  if (st) {
    // beep sound
    ;
  }
}