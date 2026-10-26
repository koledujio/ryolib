#import "AudioBackendStub.h"
#include <stdlib.h>

@implementation RYAudioBackendStub

- (instancetype)initBackendWithError:(RYError **)error {
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
    if (error) {
        *error = nil;
    }

    int *handle = (int *)malloc(sizeof(int));
    if (handle) {
        *handle = 1;
    }

    return handle;
}

- (void *)copySound:(void *)soundHandle
              error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    int *handle = (int *)malloc(sizeof(int));
    if (handle) {
        *handle = 1;
    }

    return handle;
}

- (void)destroySound:(void *)soundHandle {
    if (soundHandle) {
        free(soundHandle);
    }
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
    return 1.0;
}

- (double)soundCurrentTime:(void *)soundHandle {
    return 0.0;
}

@end
