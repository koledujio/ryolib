#pragma once

#import "../SoundInternal.h"

struct MIX_Audio;
struct MIX_Track;

@interface RYSoundSDL : RYSound {
@private
    struct MIX_Audio *_audio;
    struct MIX_Track *_track;
    unsigned int *_audioRefCount;
}
@end