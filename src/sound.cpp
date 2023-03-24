#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "sound.hpp"

Sound::Sound() : beep_{} {
}

Sound::~Sound() {
  Terminate();
}

void Sound::InitializeSound() {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    std::cerr << "Failed to initialize SDL audio 1" << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) != 0) {
    std::cerr << "Failed to initialize SDL audio 2" << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }
}

void Sound::OpenAudioFile(const char* file) {
  beep_ = Mix_LoadWAV(file);
  if (!beep_) {
    std::cerr << "Failed to open the audio file" << std::endl;
    Mix_CloseAudio();
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }
}

void Sound::Beep() {
  Mix_PlayChannel(-1, beep_, 0);
}

void Sound::StopBeep() {
  Mix_HaltChannel(-1);
}

void Sound::Terminate() {
  Mix_FreeChunk(beep_);
  Mix_CloseAudio();
  SDL_Quit();
  std::cout << "Stopped sound" << std::endl;
}