#include "AudioBackendMiniaudio.h"

namespace ryo {
    AudioEngineBackend* audio_engine_backend_miniaudio() {
        return new miniaudio_audio::MiniaudioBackend();
    }
}
