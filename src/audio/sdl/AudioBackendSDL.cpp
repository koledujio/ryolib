#include "AudioBackendSDL.h"

namespace ryo {
    AudioEngineBackend* audio_engine_backend_sdl_mixer() {
        return new sdl_mixer_audio::SdlMixerBackend();
    }
}
