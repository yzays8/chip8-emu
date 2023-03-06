# chip8-emu

## About
Simple Chip-8 emulator using SDL2 as a graphic library. NOTE: Timers are not decremented at 60Hz.
As an additional feature, you can press the "9" key to enter the sleep mode and press the "0" key to exit.

## Installation
### Linux:
```
$ sudo apt-get install libsdl2-dev
```

## Usage
Prepare the ROM in advance. Do not use copyrighted ROMs; use public domain ones.
```
$ make
$ make run ROM=<ROM>
```

## References
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/