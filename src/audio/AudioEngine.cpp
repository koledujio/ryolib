#include "private.h"

namespace ryo {
    AudioEngine::AudioEngine() : AudioEngine(AUDIO_BACKEND_AUTO) {
    }

    AudioEngine::AudioEngine(AudioBackendType type)
        : audioBackend(_audio_create_engine(type, nullptr)), backendType(type), ownsBackend(true) {
    }

    AudioEngine::AudioEngine(AudioEngineBackend* customBackend)
        : audioBackend(_audio_create_engine(AUDIO_BACKEND_CUSTOM, customBackend)),
          backendType(AUDIO_BACKEND_CUSTOM), ownsBackend(false) {
    }

    AudioEngine::~AudioEngine() {
        if (ownsBackend)
            delete audioBackend;
    }

    AudioEngine::AudioEngine(AudioEngine&& other)
        : audioBackend(other.audioBackend), backendType(other.backendType), ownsBackend(other.ownsBackend) {
        other.audioBackend = nullptr;
        other.ownsBackend = true;
    }

    AudioEngine& AudioEngine::operator=(AudioEngine&& other) {
        if (this == &other)
            return *this;

        if (ownsBackend)
            delete audioBackend;

        audioBackend = other.audioBackend;
        backendType = other.backendType;
        ownsBackend = other.ownsBackend;
        other.audioBackend = nullptr;
        other.ownsBackend = true;
        return *this;
    }

    bool AudioEngine::valid() const {
        return audioBackend != nullptr && audioBackend->valid();
    }

    AudioBackendType AudioEngine::backend() const {
        return backendType;
    }

    const char* AudioEngine::backend_name() const {
        return audioBackend ? audioBackend->name() : nullptr;
    }

    bool AudioEngine::master_volume_set(float volume) {
        RYO_ENSURE(valid(), false, "Invalid audio engine");
        return audioBackend->master_volume_set(volume);
    }
}
