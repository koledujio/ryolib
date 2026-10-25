#import "ryolib/audio/sdl/AudioEngine.h"

#include <SDL3_mixer/SDL_mixer.h>

@interface RYAudioEngineSDL (Internal)

- (MIX_Mixer *)sdlMixer;

@end
