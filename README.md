# chip8-emu

## About
Simple CHIP8 emulator using SDL2 as graphic library. Sound system is not implemented and timers are not decremented at 60Hz.

## Installation
### Linux:
```
$ sudo apt-get install libsdl2-dev
```

## Usage
Prepare ROM in advance.
```
$ g++ emu.cpp `sdl2-config --cflags --libs`
$ ./a.out <ROM>
```

## References
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/