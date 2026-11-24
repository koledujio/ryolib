#include "private.h"

namespace ryo {
    Sound::Sound(const AudioEngine& engine, const char* path, SoundLoadType loadType) {
        RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
        RYO_ENSURE_VOID(path != nullptr, "Sound path is nullptr");
        soundBackend = engine.audioBackend->sound_create(path, loadType);
    }

    Sound::Sound(const AudioEngine& engine, const Sound& source) {
        RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
        RYO_ENSURE_VOID(source.valid(), "Invalid source sound");
        soundBackend = engine.audioBackend->sound_clone(*source.soundBackend);
    }

    Sound::~Sound() {
        delete soundBackend;
    }

    Sound::Sound(Sound&& other) : soundBackend(other.soundBackend) {
        other.soundBackend = nullptr;
    }

    Sound& Sound::operator=(Sound&& other) {
        if (this == &other)
            return *this;

        delete soundBackend;
        soundBackend = other.soundBackend;
        other.soundBackend = nullptr;
        return *this;
    }

    bool Sound::valid() const {
        return soundBackend != nullptr && soundBackend->valid();
    }

    bool Sound::play() {
        RYO_ENSURE(valid(), false, "Invalid sound");
        return soundBackend->play();
    }

    bool Sound::pause() {
        RYO_ENSURE(valid(), false, "Invalid sound");
        return soundBackend->pause();
    }

    bool Sound::stop() {
        RYO_ENSURE(valid(), false, "Invalid sound");
        return soundBackend->stop();
    }

    void Sound::volume_set(float volume) {
        RYO_ENSURE_VOID(valid(), "Invalid sound");
        soundBackend->volume_set(volume);
    }

    void Sound::looping_set(bool looping) {
        RYO_ENSURE_VOID(valid(), "Invalid sound");
        soundBackend->looping_set(looping);
    }

    uint32_t Sound::duration_miliseconds_get() const {
        RYO_ENSURE(valid(), 0, "Invalid sound");
        return soundBackend->duration_miliseconds_get();
    }

    float Sound::duration_seconds_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid sound");
        return soundBackend->duration_seconds_get();
    }

    uint32_t Sound::current_miliseconds_get() const {
        RYO_ENSURE(valid(), 0, "Invalid sound");
        return soundBackend->current_miliseconds_get();
    }

    float Sound::current_seconds_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid sound");
        return soundBackend->current_seconds_get();
    }
}
