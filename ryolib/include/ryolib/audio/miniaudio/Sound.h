#pragma once

#import "../SoundInternal.h"

struct ma_sound;

@interface RYSoundMiniaudio : RYSound {
@private
    struct ma_sound *_sound;
    BOOL _paused;
}
@end