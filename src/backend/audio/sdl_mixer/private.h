#include "../private.h"

#include "ryolib/audio.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace ryo {
    struct InternalAudioEngine {
        MIX_Mixer* mixer = nullptr;
    };

    struct InternalAudio {
        MIX_Audio* audio = nullptr;
        unsigned int refCount = 0;
    };

    struct InternalSound {
        InternalAudio* audio = nullptr;
        MIX_Track* track = nullptr;
    };

    extern unsigned int _num_audio_engines;
}