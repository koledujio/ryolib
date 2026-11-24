#include "AudioBackendStub.h"

namespace ryo {
    AudioEngineBackend* audio_engine_backend_stub() {
        return new stub_audio::StubBackend();
    }
}
