#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "chip8.cpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Arguments error!" << std::endl;
    return -1;
  }

  Chip8 chip8;
  chip8.loadROM(argv[1]);

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);
  SDL_SetWindowTitle(window, "CHIP8-EMULATOR");
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  SDL_Rect pixel {
    0,  // x (temporary)
    0,  // y (temporary)
    10, // width
    10, // height
  };

  for (;;) {
    chip8.runLoop();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          std::cout << "Shutdown..." << std::endl;
          SDL_DestroyRenderer(renderer);
          SDL_DestroyWindow(window);
          SDL_Quit();
          return 0;
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_1:
              chip8.key[0x1] = 1;
              break;
            case SDLK_2:
              chip8.key[0x2] = 1;
              break;
            case SDLK_3:
              chip8.key[0x3] = 1;
              break;
            case SDLK_4:
              chip8.key[0xC] = 1;
              break;
            case SDLK_q:
              chip8.key[0x4] = 1;
              break;
            case SDLK_w:
              chip8.key[0x5] = 1;
              break;
            case SDLK_e:
              chip8.key[0x6] = 1;
              break;
            case SDLK_r:
              chip8.key[0xD] = 1;
              break;
            case SDLK_a:
              chip8.key[0x7] = 1;
              break;
            case SDLK_s:
              chip8.key[0x8] = 1;
              break;
            case SDLK_d:
              chip8.key[0x9] = 1;
              break;
            case SDLK_f:
              chip8.key[0xE] = 1;
              break;
            case SDLK_z:
              chip8.key[0xA] = 1;
              break;
            case SDLK_x:
              chip8.key[0x0] = 1;
              break;
            case SDLK_c:
              chip8.key[0xB] = 1;
              break;
            case SDLK_v:
              chip8.key[0xF] = 1;
              break;
          }
          break;
        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            case SDLK_1:
              chip8.key[0x1] = 0;
              break;
            case SDLK_2:
              chip8.key[0x2] = 0;
              break;
            case SDLK_3:
              chip8.key[0x3] = 0;
              break;
            case SDLK_4:
              chip8.key[0xC] = 0;
              break;
            case SDLK_q:
              chip8.key[0x4] = 0;
              break;
            case SDLK_w:
              chip8.key[0x5] = 0;
              break;
            case SDLK_e:
              chip8.key[0x6] = 0;
              break;
            case SDLK_r:
              chip8.key[0xD] = 0;
              break;
            case SDLK_a:
              chip8.key[0x7] = 0;
              break;
            case SDLK_s:
              chip8.key[0x8] = 0;
              break;
            case SDLK_d:
              chip8.key[0x9] = 0;
              break;
            case SDLK_f:
              chip8.key[0xE] = 0;
              break;
            case SDLK_z:
              chip8.key[0xA] = 0;
              break;
            case SDLK_x:
              chip8.key[0x0] = 0;
              break;
            case SDLK_c:
              chip8.key[0xB] = 0;
              break;
            case SDLK_v:
              chip8.key[0xF] = 0;
              break;
          }
          break;
      }
    }

    if (chip8.isDrew) {
      chip8.isDrew = false;
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
          pixel.x = 10 * j;
          pixel.y = 10 * i;
          if (chip8.frameBuf[i][j] == 1) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
          } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
          }
          SDL_RenderFillRect(renderer, &pixel);
        }
      }
      SDL_RenderPresent(renderer);  // This function should not be placed in the loop
    }

    usleep(1200); // Can change game speed
  }
}