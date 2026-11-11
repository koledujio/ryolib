#pragma once

#include "templates.h"

namespace ryo {
    struct InternalAudioEngine;
    struct InternalSound;

    struct SoundDesc;

    struct AudioEngine {
        InternalAudioEngine* internal = nullptr;

        AudioEngine();
        ~AudioEngine();

        AudioEngine(const AudioEngine&) = delete;
        AudioEngine& operator=(const AudioEngine&) = delete;

        AudioEngine(AudioEngine&&);
        AudioEngine& operator=(AudioEngine&&);

        bool valid() const;

        bool master_volume_set(float volume);
    };

    struct Sound {
        InternalSound* internal = nullptr;

        Sound(const AudioEngine& engine, SoundDesc soundDesc); // load
        Sound(const AudioEngine& engine, const Sound& source); // clone(reuse existing memory)
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

    enum SoundLoadType {
        SOUND_LOAD_TYPE_DECODE,
        SOUND_LOAD_TYPE_STREAM
    };

    struct SoundConfig {
        float volume = 1.0f;
        bool looping = false;
    };

    struct SoundDesc {
        const char* path = nullptr;
        SoundLoadType loadType = SOUND_LOAD_TYPE_DECODE;
        SoundConfig config;
    };
}