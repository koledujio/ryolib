#pragma once

#include "../templates.h"

namespace ryo {
    enum AudioBackendType {
        AUDIO_BACKEND_AUTO,
        AUDIO_BACKEND_MINIAUDIO,
        AUDIO_BACKEND_SDL_MIXER,
        AUDIO_BACKEND_STUB,
        AUDIO_BACKEND_CUSTOM
    };

    enum SoundLoadType {
        SOUND_LOAD_TYPE_DECODE,
        SOUND_LOAD_TYPE_STREAM
    };

    struct SoundBackend {
        virtual ~SoundBackend() = default;

        virtual bool valid() const = 0;
        virtual bool play() = 0;
        virtual bool pause() = 0;
        virtual bool stop() = 0;
        virtual void volume_set(float volume) = 0;
        virtual void looping_set(bool looping) = 0;
        virtual uint32_t duration_miliseconds_get() const = 0;
        virtual float duration_seconds_get() const = 0;
        virtual uint32_t current_miliseconds_get() const = 0;
        virtual float current_seconds_get() const = 0;
    };

    struct AudioEngineBackend {
        virtual ~AudioEngineBackend() = default;

        virtual const char* name() const = 0;
        virtual bool is_supported() const = 0;
        virtual bool valid() const = 0;
        virtual bool master_volume_set(float volume) = 0;
        virtual SoundBackend* sound_create(const char* path, SoundLoadType loadType) = 0;
        virtual SoundBackend* sound_clone(const SoundBackend& source) = 0;
    };
}
