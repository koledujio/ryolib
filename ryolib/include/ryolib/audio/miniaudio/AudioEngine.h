#pragma once

#import "../AudioEngineInternal.h"

struct ma_engine;

@interface RYAudioEngineMiniaudio : RYAudioEngine {
@private
    struct ma_engine *_engine;
}
@end
