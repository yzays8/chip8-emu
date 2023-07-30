# chip8-emu

## About

This is a simple but full-featured CHIP-8 emulator.

## Features

- The main system runs at 500 Hz and the timers run at 60 Hz.
- The sound system works.
- Press <kbd>Space</kbd> to sleep.
- Press <kbd>T</kbd> to advance one CPU cycle during sleep.
- Press <kbd>9</kbd> / <kbd>0</kbd> to change the objects / background color.

## Requirement

SDL2 is required as a graphics and sound library.

### Debian (Ubuntu)

```sh
sudo apt install libsdl2-dev
```

### Mac

```sh
brew install sdl2 sdl2_mixer
```

## Build

```sh
make
```

## Run

### Standard mode

```sh
make run ROM=<rom_path>
```

### Debug mode (Show current state)

```sh
make debug ROM=<rom_path>
```

## Key bindings

### Original Chip8 keyboard

| | | | |
|-|-|-|-|
|1|2|3|C|
|4|5|6|D|
|7|8|9|E|
|A|0|B|F|

### Emulated keyboard

| | | | |
|-|-|-|-|
|1|2|3|4|
|Q|W|E|R|
|A|S|D|F|
|Z|X|C|V|

## References

- <http://devernay.free.fr/hacks/chip8/C8TECH10.HTM>
