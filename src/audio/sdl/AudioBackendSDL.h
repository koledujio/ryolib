#pragma once

#include "../private.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace ryo {
    namespace sdl_mixer_audio {
        struct SdlMixerBackend : AudioEngineBackend {
            MIX_Mixer* mixer = nullptr;
            bool initialized = false;

            SdlMixerBackend();
            ~SdlMixerBackend() override;

            const char* name() const override { return "sdl_mixer"; }
            bool is_supported() const override { return true; }
            bool valid() const override;
            bool master_volume_set(float volume) override;
            SoundBackend* sound_create(const char* path, SoundLoadType loadType) override;
            SoundBackend* sound_clone(const SoundBackend& source) override;
        };

        struct SdlMixerAudio {
            MIX_Audio* audio = nullptr;
            unsigned int refCount = 0;
        };

        struct SdlMixerSound : SoundBackend {
            SdlMixerAudio* audio = nullptr;
            MIX_Track* track = nullptr;

            SdlMixerSound(SdlMixerBackend& engine, const char* path, SoundLoadType loadType);
            SdlMixerSound(SdlMixerBackend& engine, const SdlMixerSound& source);
            ~SdlMixerSound() override;

            bool valid() const override;
            bool play() override;
            bool pause() override;
            bool stop() override;
            void volume_set(float volume) override;
            void looping_set(bool looping) override;
            uint32_t duration_miliseconds_get() const override;
            float duration_seconds_get() const override;
            uint32_t current_miliseconds_get() const override;
            float current_seconds_get() const override;
        };
    }
}
