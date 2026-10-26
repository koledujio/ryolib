#pragma once

#import "ryolib/audio/AudioBackend.h"

struct MIX_Mixer;

@interface RYAudioBackendSDL : RYAudioBackend {
@private
    struct MIX_Mixer *_mixer;
}

- (struct MIX_Mixer *)sdlMixer;

@end
