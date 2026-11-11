#include "ryolib/audio.h"

namespace ryo {
    struct InternalSound {};

    Sound::Sound(const AudioEngine& engine, SoundDesc soundDesc) {}
    Sound::Sound(const AudioEngine& engine, const Sound& source) {}
    Sound::~Sound() {}

    Sound::Sound(Sound&&) {}
    Sound& Sound::operator=(Sound&&) {return *this;}

    bool Sound::valid() const {return true;}

    bool Sound::play() {return true;}
    bool Sound::pause() {return true;}
    bool Sound::stop() {return true;}

    void Sound::volume_set(float volume) {}
    void Sound::looping_set(bool looping) {}

    uint32_t Sound::duration_miliseconds_get() const {return 0;}
    float Sound::duration_seconds_get() const {return 0.0f;}
    uint32_t Sound::current_miliseconds_get() const {return 0;}
    float Sound::current_seconds_get() const {return 0.0f;}
}