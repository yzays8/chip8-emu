#include <unistd.h>
#include <iostream>
#include <memory>

#include "chip8.hpp"

constexpr int kWindowScale = 15;  // change window size

int main(int argc, char** argv) {
  if (argc != 2 && argc != 3) {
    std::cerr << "Arguments error" << std::endl;
    return 1;
  }

  opterr = 0;

  bool debug_mode = false;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
      case 'd':
        debug_mode = true;
        break;
      default:
        std::cout << "Usage: " << argv[0] << " [-d] <rom_path>" << std::endl;
        return 1;
    }
  }

  auto chip8 = std::make_unique<chip8_emu::Chip8>(debug_mode);

  chip8->LoadROM(argv[optind]);
  chip8->InitializeWindow(kWindowScale);
  bool success = chip8->Run();
  if (!success) {
    std::cerr << "Exit with error" << std::endl;
    return 1;
  }
}
