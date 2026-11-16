#include "ryolib/audio.h"

namespace ryo {
    struct InternalAudioEngine {};

    AudioEngine::AudioEngine() {}
    AudioEngine::~AudioEngine() {}

    AudioEngine::AudioEngine(AudioEngine&&) {}
    AudioEngine& AudioEngine::operator=(AudioEngine&&) {return *this;}

    bool AudioEngine::valid() const {return true;}

    bool AudioEngine::master_volume_set(float volume) {return true;}
}