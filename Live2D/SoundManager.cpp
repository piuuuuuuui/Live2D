#define DR_MP3_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#include "SoundManager.hpp"

SoundManager::SoundManager()
    : _pcmData(NULL), _timeSec(0.f), _rms(0.f), _lastSample(0) {}

SoundManager::~SoundManager() {
  if (_pcmData != NULL) drwav_free(_pcmData, NULL);
}

void SoundManager::init() {
  if (_pcmData != NULL) drwav_free(_pcmData, NULL);
  _pcmData = NULL;
  _timeSec = 0.f;
  _rms = 0.f;
  _lastSample = 0;
}

bool SoundManager::start(const char* filePath) {
  if (strcmp(filePath, "") == 0) return false;
  init();
  const char* fileExtension = strrchr(filePath, '.');
  if (strcmp(fileExtension, ".mp3") == 0) {
    drmp3 mp3;
    if (drmp3_init_file(&mp3, filePath, NULL)) {
      _pcmData = drmp3__full_read_and_close_f32(&mp3, NULL, &_totSamples);
      _channels = mp3.channels;
      _sampleRate = mp3.sampleRate;
    }
  } else if (strcmp(fileExtension, ".wav") == 0) {
    _pcmData = drwav_open_file_and_read_pcm_frames_f32(
        filePath, &_channels, &_sampleRate, &_totSamples, NULL);
  }
  return true;
}

bool SoundManager::update(const float deltaTimeSec) {
  if (_pcmData == NULL) return false;

  _timeSec += deltaTimeSec;
  _sample = static_cast<size_t>(_timeSec * _sampleRate);
  if (_sample <= _totSamples) {
    _rms = 0;
    for (size_t sampleCnt = _lastSample; sampleCnt < _sample; sampleCnt++) {
      for (size_t channelCnt = 0; channelCnt < _channels; channelCnt++) {
        float pcm = _pcmData[sampleCnt * _channels + channelCnt];
        _rms += pcm * pcm;
      }
    }
    _rms = sqrt(_rms / ((_sample - _lastSample) * _channels));
    _lastSample = _sample;
    return true;
  }
  init();
  return false;
}

float SoundManager::getRms() { return _rms; }