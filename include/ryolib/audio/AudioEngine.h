#pragma once

#include "AudioBackend.h"

namespace ryo {
    struct AudioEngine {
        AudioEngineBackend* audioBackend = nullptr;
        AudioBackendType backendType = AUDIO_BACKEND_AUTO;
        bool ownsBackend = true;

        AudioEngine();
        explicit AudioEngine(AudioBackendType backendType);
        explicit AudioEngine(AudioEngineBackend* customBackend);
        ~AudioEngine();

        AudioEngine(const AudioEngine&) = delete;
        AudioEngine& operator=(const AudioEngine&) = delete;

        AudioEngine(AudioEngine&&);
        AudioEngine& operator=(AudioEngine&&);

        bool valid() const;
        AudioBackendType backend() const;
        const char* backend_name() const;

        bool master_volume_set(float volume);
    };
}
