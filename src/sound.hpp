#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

namespace chip8_emu {

extern const std::string kBeepFilePath;

class Sound {
 public:
  Sound();
  ~Sound();
  void InitializeSound();
  void Beep();
  void StopBeep();
  void Terminate();

 private:
  void OpenAudioFile(const std::string& file);

  Mix_Chunk* beep_;
};

} // namespace chip8_emu
