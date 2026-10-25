#if defined(RYO_ENABLE_AUDIO_MINIAUDIO)

#import "AudioEngineInternal.h"

#include <stdlib.h>

@implementation RYAudioEngineMiniaudio

- (instancetype)initBackendWithError:(RYError **)error {
    self = [super initCommon];

    if (!self)
        return nil;
    
    _backend = RYAudioEngineBackendMiniaudio;

    _engine = malloc(sizeof(ma_engine));
    if (!_engine) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Failed to allocate miniaudio engine"];
        [self release];
        return nil;
    }
    
    ma_result result = ma_engine_init(NULL, _engine);

    if (result != MA_SUCCESS) {
        free(_engine);
        _engine = NULL;
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(result)];
        [self release];
        return nil;
    }

    return self;
}

- (void)dealloc {
    if (_engine) {
        ma_engine_uninit(_engine);
        free(_engine);
        _engine = NULL;
    }

    [super dealloc];
}

- (BOOL)applyBackendMasterVolume:(float)volume
                           error:(RYError **)error {
    if (!_engine) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Audio engine is not initialized"];
        return NO;
    }

    ma_result result = ma_engine_set_volume(_engine, volume);

    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(result)];
        return NO;
    }

    return YES;
}

- (ma_engine *)miniaudioEngine {
    return _engine;
}

@end

#else

@implementation RYAudioEngineMiniaudio

- (instancetype)initBackendWithError:(RYError **)error {
    if (error)
        *error = [[RYError alloc] initWithCode:-1 message:"Unsupported platform"];

    return nil;
}

@end

#endif
