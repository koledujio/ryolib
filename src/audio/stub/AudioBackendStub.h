#pragma once

#include "../private.h"

namespace ryo {
    namespace stub_audio {
        struct StubSound : SoundBackend {
            bool valid() const override { return true; }
            bool play() override { return true; }
            bool pause() override { return true; }
            bool stop() override { return true; }
            void volume_set(float volume) override { (void)volume; }
            void looping_set(bool looping) override { (void)looping; }
            uint32_t duration_miliseconds_get() const override { return 0; }
            float duration_seconds_get() const override { return 0.0f; }
            uint32_t current_miliseconds_get() const override { return 0; }
            float current_seconds_get() const override { return 0.0f; }
        };

        struct StubBackend : AudioEngineBackend {
            const char* name() const override { return "stub"; }
            bool is_supported() const override { return true; }
            bool valid() const override { return true; }
            bool master_volume_set(float volume) override { (void)volume; return true; }
            SoundBackend* sound_create(const char* path, SoundLoadType loadType) override {
                (void)path;
                (void)loadType;
                return new StubSound();
            }
            SoundBackend* sound_clone(const SoundBackend& source) override {
                (void)source;
                return new StubSound();
            }
        };
    }
}
