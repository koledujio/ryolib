#import "ryolib/audio/AudioEngine.h"

#import "stub/AudioBackendStub.h"
#import "miniaudio/AudioBackendMiniaudio.h"
#import "sdl/AudioBackendSDL.h"

@implementation RYAudioEngine

@synthesize backend = _backend;
@synthesize masterVolume = _masterVolume;
@synthesize backendInstance = _backendInstance;

+ (instancetype)newEngineWithBackend:(RYAudioEngineBackend)backend {
    return [self newEngineWithBackend:backend error:NULL];
}

+ (instancetype)newEngineWithBackend:(RYAudioEngineBackend)backend
                               error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    Class backendClass = Nil;

    switch (backend) {
        case RYAudioEngineBackendDefault:
#if defined(RYO_ENABLE_AUDIO_MINIAUDIO)
            backendClass = [RYAudioBackendMiniaudio class];
            backend = RYAudioEngineBackendMiniaudio;
#elif defined(RYO_ENABLE_AUDIO_SDL)
            backendClass = [RYAudioBackendSDL class];
            backend = RYAudioEngineBackendSDL3;
#else
            backendClass = [RYAudioBackendStub class];
            backend = RYAudioEngineBackendStub;
#endif
            break;

        case RYAudioEngineBackendStub:
            backendClass = [RYAudioBackendStub class];
            break;

        case RYAudioEngineBackendMiniaudio:
            backendClass = [RYAudioBackendMiniaudio class];
            break;

        case RYAudioEngineBackendSDL3:
            backendClass = [RYAudioBackendSDL class];
            break;

        default:
            break;
    }

    if (backendClass == Nil) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioInvalidBackend message:"Invalid or unsupported audio backend"];
        return nil;
    }

    RYAudioBackend *backendInstance = [[backendClass alloc] initBackendWithError:error];
    if (!backendInstance) {
        return nil;
    }

    RYAudioEngine *engine = [[RYAudioEngine alloc] initWithBackendInstance:backendInstance
                                                               backendType:backend];
    [backendInstance release];
    return engine;
}

+ (instancetype)newEngineWithCustomBackend:(RYAudioBackend *)customBackend {
    return [self newEngineWithCustomBackend:customBackend error:NULL];
}

+ (instancetype)newEngineWithCustomBackend:(RYAudioBackend *)customBackend
                                     error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!customBackend) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioInvalidBackend message:"Custom backend is nil"];
        return nil;
    }

    return [[RYAudioEngine alloc] initWithBackendInstance:customBackend
                                              backendType:RYAudioEngineBackendCustom];
}

- (instancetype)initWithBackendInstance:(RYAudioBackend *)backendInstance
                            backendType:(RYAudioEngineBackend)backend {
    self = [super init];
    if (!self) {
        return nil;
    }

    _backend = backend;
    _masterVolume = 1.0f;
    _backendInstance = [backendInstance retain];

    return self;
}

- (void)dealloc {
    [_backendInstance release];
    [super dealloc];
}

- (BOOL)setMasterVolume:(float)masterVolume {
    return [self setMasterVolume:masterVolume error:NULL];
}

- (BOOL)setMasterVolume:(float)masterVolume
                  error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (masterVolume < 0.0f) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioInvalidVolume message:"Invalid master volume amount (below 0.0f)"];
        return NO;
    }

    if (![_backendInstance applyMasterVolume:masterVolume error:error]) {
        return NO;
    }

    _masterVolume = masterVolume;
    return YES;
}

@end
