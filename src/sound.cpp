#include <iostream>
#include <filesystem>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "sound.hpp"

namespace chip8_emu {

const std::string kBeepFilePath{"../sound/beep.wav"};

Sound::Sound() : beep_{nullptr} {}

Sound::~Sound() {
  Terminate();
}

void Sound::InitializeSound() {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    std::cerr << "Failed to initialize SDL audio: " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) != 0) {
    std::cerr << "Failed to open SDL mixer" << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }

  OpenAudioFile(kBeepFilePath);
}

void Sound::OpenAudioFile(const std::string& file) {
  namespace fs = std::filesystem;
  beep_ = Mix_LoadWAV(file.c_str());
  if (!beep_) {
    std::cerr << "Failed to open the audio file: " << fs::weakly_canonical(fs::absolute(file)) << std::endl;
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

} // namespace chip8_emu
