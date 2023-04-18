# chip8-emu

## About

Simple Chip-8 emulator with SDL2 as a graphics and sound library.

## Features

- The main system runs at 500 Hz and the timers run at 60 Hz.
- The sound system is implemented.
- Press <kbd>Space</kbd> to enter the sleep mode. Press it again to exit.
- Press <kbd>9</kbd> or <kbd>0</kbd> to change the objects/background color.
- It has a debug mode.

## Installation

### Linux

```sh
$ sudo apt-get install libsdl2-dev
```

## Usage

Prepare the ROM in advance.

### standard mode

```sh
$ make
$ make run ROM=<rom_path>
```

### debug mode

```sh
$ make
$ make debug ROM=<rom_path>
```

## Key bindings

#### Original Chip8 keyboard

| | | | |
|-|-|-|-|
|1|2|3|C|
|4|5|6|D|
|7|8|9|E|
|A|0|B|F|

#### Emulated keyboard

| | | | |
|-|-|-|-|
|1|2|3|4|
|Q|W|E|R|
|A|S|D|F|
|Z|X|C|V|

## References
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/