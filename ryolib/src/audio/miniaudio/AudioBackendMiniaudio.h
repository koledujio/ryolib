#pragma once

#import "ryolib/audio/AudioBackend.h"

struct ma_engine;

@interface RYAudioBackendMiniaudio : RYAudioBackend {
@private
    struct ma_engine *_engine;
}

- (struct ma_engine *)miniaudioEngine;

@end
