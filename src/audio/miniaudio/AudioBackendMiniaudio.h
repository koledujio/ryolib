#pragma once

#include "../private.h"

#include "ryolib/templates.h"

#include "vendor/miniaudio.h"

namespace ryo {
    namespace miniaudio_audio {
        struct MiniaudioBackend : AudioEngineBackend {
            ma_engine* engine = nullptr;

            MiniaudioBackend();
            ~MiniaudioBackend() override;

            const char* name() const override { return "miniaudio"; }
            bool is_supported() const override { return true; }
            bool valid() const override;
            bool master_volume_set(float volume) override;
            SoundBackend* sound_create(const char* path, SoundLoadType loadType) override;
            SoundBackend* sound_clone(const SoundBackend& source) override;
        };

        struct MiniaudioSound : SoundBackend {
            ma_sound* sound = nullptr;

            MiniaudioSound(MiniaudioBackend& engine, const char* path, SoundLoadType loadType);
            MiniaudioSound(MiniaudioBackend& engine, const MiniaudioSound& source);
            ~MiniaudioSound() override;

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

        ma_uint32 _sound_get_flags(SoundLoadType loadType);
    }
}
