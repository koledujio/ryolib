#pragma once

#include "../private.h"

#include "ryolib/audio.h"

namespace ryo {
    AudioEngineBackend* audio_engine_backend_miniaudio();
    AudioEngineBackend* audio_engine_backend_sdl_mixer();
    AudioEngineBackend* audio_engine_backend_stub();

    AudioEngineBackend* _audio_create_engine(AudioBackendType backendType, AudioEngineBackend* customBackend);
}
