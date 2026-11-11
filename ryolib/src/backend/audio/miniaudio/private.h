#pragma once

#include "../private.h"

#include "ryolib/audio.h"
#include "ryolib/templates.h"

#include "vendor/miniaudio.h"

namespace ryo {
    struct InternalAudioEngine {
        ma_engine* engine = nullptr;
    };

    struct InternalSound {
        ma_sound* sound = nullptr;
    };

    ma_uint32 _sound_get_flags(SoundLoadType loadType);
    void _configure_sound(ma_sound* sound, SoundConfig config);
}