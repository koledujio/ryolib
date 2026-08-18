#include "../private.h"

namespace ryo {
    ma_uint32 _sound_get_flags(SoundLoadType loadType) {
        ma_uint32 flags = 0;

        switch (loadType) {
            case SOUND_LOAD_TYPE_DECODE:
                flags |= MA_SOUND_FLAG_DECODE;
                break;
            case SOUND_LOAD_TYPE_STREAM:
                flags |= MA_SOUND_FLAG_STREAM;
                break;
        }

        return flags;
    }

    void _configure_sound(ma_sound* sound, SoundConfig config) {
        ma_sound_set_volume(sound, config.volume);
        ma_sound_set_looping(sound, config.looping ? MA_TRUE : MA_FALSE);
    }
}