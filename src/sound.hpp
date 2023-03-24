#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

const char kBeepFilePath[] = "../sound/beep.wav";

class Sound {
 public:
  Sound();
  ~Sound();
  void InitializeSound();
  void OpenAudioFile(const char* file);
  void Beep();
  void StopBeep();
  void Terminate();

 private:
  Mix_Chunk* beep_;
};