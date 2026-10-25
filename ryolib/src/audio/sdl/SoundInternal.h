#pragma once

#import "ryolib/audio/sdl/Sound.h"

#include <SDL3_mixer/SDL_mixer.h>

@interface RYSoundSDL (Internal)

- (MIX_Audio *)sdlAudio;
- (MIX_Track *)sdlTrack;
- (unsigned int *)audioRefCount;

@end
