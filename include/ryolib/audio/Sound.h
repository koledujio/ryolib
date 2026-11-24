#pragma once

#include "AudioBackend.h"
#include "AudioEngine.h"

namespace ryo {
    struct Sound {
        SoundBackend* soundBackend = nullptr;

        Sound(const AudioEngine& engine, const char* path,
              SoundLoadType loadType = SOUND_LOAD_TYPE_DECODE);
        Sound(const AudioEngine& engine, const Sound& source); // reuse existing memory
        ~Sound();

        Sound(const Sound&) = delete;
        Sound& operator=(const Sound&) = delete;

        Sound(Sound&&);
        Sound& operator=(Sound&&);

        bool valid() const;

        bool play();
        bool pause();
        bool stop();

        void volume_set(float volume);
        void looping_set(bool looping);

        uint32_t duration_miliseconds_get() const;
        float duration_seconds_get() const;
        uint32_t current_miliseconds_get() const;
        float current_seconds_get() const;
    };
}
