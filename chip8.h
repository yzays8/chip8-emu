#ifndef CHIP8_H_
#define CHIP8_H_

#include <cstdint>

class Chip8 {
 public:
  Chip8();
  uint8_t mem[4096];
  int frameBuf[32][64];
  bool key[16];
  bool isDrawn;

  void loadROM(const std::string rom);
  void interpretInstruction(const uint16_t ins);
  void runLoop();

  ~Chip8();

 private:
  uint8_t v[16];
  uint16_t i;
  uint16_t pc;
  uint8_t sp;
  uint16_t stack[16];
  uint8_t dt;
  uint8_t st;
};

#endif