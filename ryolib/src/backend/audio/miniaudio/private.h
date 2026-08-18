#pragma once

#include "../private.h"

#include "ryolib/audio.h"
#include "ryolib/templates.h"

#include "vendor/miniaudio.h"

namespace ryo {
    constexpr unsigned int MAX_ENGINES = 4;
    constexpr unsigned int MAX_SOUNDS = 1024;

    extern ResourcePool<ma_engine, MAX_ENGINES> _engine_pool;
    extern ResourcePool<ma_sound, MAX_SOUNDS> _sound_pool;

    ma_uint32 _sound_get_flags(SoundLoadType loadType);
    void _configure_sound(ma_sound* sound, SoundConfig config);
}