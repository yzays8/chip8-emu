# chip8-emu

## About
Simple CHIP8 emulator using SDL2 as graphic library. Timers are not decremented at 60Hz.
As an additional feature, you can press "9" key to enter the sleep mode and press "0" key to exit.

## Installation
### Linux:
```
$ sudo apt-get install libsdl2-dev
```

## Usage
Prepare ROM in advance.
```
$ make
$ ./emu <ROM>
```

## References
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/