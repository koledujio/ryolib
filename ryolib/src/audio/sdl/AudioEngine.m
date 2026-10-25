#if defined(RYO_ENABLE_AUDIO_SDL)

#import "AudioEngineInternal.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>

static unsigned int g_numAudioEngines = 0;

@implementation RYAudioEngineSDL

- (instancetype)initBackendWithError:(RYError **)error {
    self = [super initCommon];

    if (!self)
        return nil;
    
    _backend = RYAudioEngineBackendSDL3;
    _mixer = NULL;
    
    if (g_numAudioEngines == 0) {
        if (!MIX_Init()) {
            if (error)
                *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
            [self release];
            return nil;
        }
    }
    
    _mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    if (!_mixer) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];

        if (g_numAudioEngines == 0) {
            MIX_Quit();
        }

        [self release];
        return nil;
    }

    g_numAudioEngines++;

    return self;
}

- (void)dealloc {
    if (_mixer) {
        MIX_DestroyMixer(_mixer);
        _mixer = NULL;

        if (g_numAudioEngines == 1) {
            MIX_Quit();
        }

        g_numAudioEngines--;
    }

    [super dealloc];
}

- (BOOL)applyBackendMasterVolume:(float)volume
                           error:(RYError **)error {
    if (!_mixer) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Audio engine is not initialized"];
        return NO;
    }

    if (!MIX_SetMixerGain(_mixer, volume)) {
        *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (MIX_Mixer *)sdlMixer {
    return _mixer;
}

@end

#else

@implementation RYAudioEngineSDL

- (instancetype)initBackendWithError:(RYError **)error {
    if (error)
        *error = [[RYError alloc] initWithCode:-1 message:"Unsupported platform"];

    return nil;
}

@end

#endif