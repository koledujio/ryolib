#include "../AudioBackendMiniaudio.h"

namespace ryo {
    namespace miniaudio_audio {
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
    }
}
