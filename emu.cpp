#include <iostream>

#include "chip8.hpp"

const int window_scale = 15;  // change window size

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Arguments error" << std::endl;
    return EXIT_FAILURE;
  }

  Chip8 chip8;

  chip8.LoadROM(argv[1]);
  chip8.InitializeWindow(window_scale);
  chip8.RunLoop();
}