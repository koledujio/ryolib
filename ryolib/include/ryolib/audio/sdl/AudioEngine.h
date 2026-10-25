#pragma once

#import "../AudioEngineInternal.h"

struct MIX_Mixer;

@interface RYAudioEngineSDL : RYAudioEngine {
@private
    struct MIX_Mixer *_mixer;
}
@end
