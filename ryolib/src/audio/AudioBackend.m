#import "ryolib/audio/AudioBackend.h"

@implementation RYAudioBackend

- (instancetype)initBackendWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    return [super init];
}

- (BOOL)applyMasterVolume:(float)volume
                    error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    return YES;
}

- (void *)createSoundWithPath:(RYString *)path
                     loadType:(RYSoundLoadType)loadType
                        error:(RYError **)error {
    if (error)
        *error = [[RYError alloc] initWithCode:-1 message:"Audio backend does not support creating sounds"];
    
    return NULL;
}

- (void *)copySound:(void *)soundHandle
              error:(RYError **)error {
    if (error)
        *error = [[RYError alloc] initWithCode:-1 message:"Audio backend does not support copying sounds"];
    
    return NULL;
}

- (void)destroySound:(void *)soundHandle {
}

- (BOOL)playSound:(void *)soundHandle
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    
    return YES;
}

- (BOOL)pauseSound:(void *)soundHandle
             error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    
    return YES;
}

- (BOOL)stopSound:(void *)soundHandle
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    
    return YES;
}

- (BOOL)seekSound:(void *)soundHandle
           toTime:(double)time
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    
    return YES;
}

- (BOOL)setSound:(void *)soundHandle
          volume:(float)volume
           error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    
    return YES;
}

- (BOOL)setSound:(void *)soundHandle
         looping:(BOOL)looping
           error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    
    return YES;
}

- (RYSoundPlaybackState)soundPlaybackState:(void *)soundHandle {
    return RYSoundPlaybackStateStopped;
}

- (double)soundDuration:(void *)soundHandle {
    return 0.0;
}

- (double)soundCurrentTime:(void *)soundHandle {
    return 0.0;
}

@end
