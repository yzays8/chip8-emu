#include <iostream>
#include <memory>

#include "chip8.hpp"

const int kWindowScale = 15;  // change window size

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Arguments error" << std::endl;
    return 1;
  }

  auto chip8 = std::make_unique<chip8_emu::Chip8>(atoi(argv[2]));

  chip8->LoadROM(argv[1]);
  chip8->InitializeWindow(kWindowScale);
  chip8->RunLoop();
}