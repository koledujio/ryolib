#import "ryolib/audio/AudioEngineInternal.h"

#import "ryolib/audio/stub/AudioEngine.h"
#import "ryolib/audio/miniaudio/AudioEngine.h"
#import "ryolib/audio/sdl/AudioEngine.h"

@implementation RYAudioEngine

@synthesize backend = _backend;
@synthesize masterVolume = _masterVolume;

+ (instancetype)engineWithBackend:(RYAudioEngineBackend)backend
                            error:(RYError **)error {
    return [self newEngineWithBackend:backend error:error];
}

+ (instancetype)newEngineWithBackend:(RYAudioEngineBackend)backend
                            error:(RYError **)error {
    Class engineClass = Nil;

    switch (backend) {
        case RYAudioEngineBackendDefault:
#if defined(RYO_ENABLE_AUDIO_MINIAUDIO)
            engineClass = [RYAudioEngineMiniaudio class];
#elif defined(RYO_ENABLE_AUDIO_SDL)
            engineClass = [RYAudioEngineSDL class];
#else
            engineClass = [RYAudioEngineStub class];
#endif
            break;
        
        case RYAudioEngineBackendStub:
            engineClass = [RYAudioEngineStub class];
            break;
        
        case RYAudioEngineBackendMiniaudio:
            engineClass = [RYAudioEngineMiniaudio class];
            break;
        
        case RYAudioEngineBackendSDL3:
            engineClass = [RYAudioEngineSDL class];
            break;
        
        default:
            break;
    }

    if (engineClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid audio backend"];
        }
        
        return nil;
    }

    return [[engineClass alloc] initBackendWithError:error];
}

- (BOOL)setMasterVolume:(float)masterVolume
                  error:(RYError **)error {
    if (masterVolume < 0.0f) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid master volume amount (below 0.0f)"];
        return NO;
    }

    if (![self applyBackendMasterVolume:masterVolume error:error]) {
        return NO;
    }

    _masterVolume = masterVolume;
    return YES;
}

- (instancetype)initCommon {
    self = [super init];

    if (!self) {
        return nil;
    }

    _masterVolume = 1.0f;

    return self;
}

- (instancetype)initBackendWithError:(RYError **)error {
    return nil;
}

- (BOOL)applyBackendMasterVolume:(float)volume
                           error:(RYError **)error {
    return NO;
}

@end
