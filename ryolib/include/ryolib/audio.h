#pragma once

#include "templates.h"

namespace ryo {
    using AudioEngine = Handle<struct AudioEngineTag>;
    using Sound = Handle<struct SoundTag>;

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

    AudioEngine audio_engine_create();
    void audio_engine_destroy(AudioEngine engine);
    bool audio_engine_is_valid(AudioEngine engine);
    bool audio_engine_set_master_volume(AudioEngine engine, float volume);

    Sound sound_load(AudioEngine engine, SoundDesc soundDesc);
    Sound sound_clone(AudioEngine engine, Sound source);
    void sound_destroy(Sound sound);
    bool sound_is_valid(Sound sound);

    bool sound_play(Sound sound);
    bool sound_pause(Sound sound);
    bool sound_stop(Sound sound);

    void sound_set_volume(Sound sound, float volume);
    void sound_set_looping(Sound sound, bool looping);

    uint32_t sound_get_duration_miliseconds(Sound sound);
    uint32_t sound_get_current_miliseconds(Sound sound);
    float sound_get_duration_seconds(Sound sound);
    float sound_get_current_seconds(Sound sound);
}