#pragma once

#include "dr_flac.h"
#include "dr_mp3.h"
#include "dr_wav.h"

class SoundManager {
 public:
   SoundManager();
   ~SoundManager();
  void init();
  bool start(const char* filePath);
  bool update(const float deltaTimeSec);
  float getRms();

 private:
  unsigned int _channels;
  unsigned int _sampleRate;
  size_t _totSamples;
  float* _pcmData;

  size_t _lastSample;
  size_t _sample;
  float _timeSec;
  float _rms;
};
