#import "ryolib/audio/stub/Sound.h"
#import "ryolib/audio/SoundInternal.h"

@implementation RYSoundStub

- (instancetype)backendInitWithEngine:(RYAudioEngine *)audioEngine
                                 path:(RYString *)path
                             loadType:(RYSoundLoadType)loadType
                                error:(RYError **)error {
    self = [super initCommonWithEngine:audioEngine
                                  path:path
                              loadType:loadType];
    
    if (!self) {
        return nil;
    }

    return self;
}

- (instancetype)backendInitCopyOfSound:(RYSound *)sound
                                 error:(RYError **)error {
    self = [super initCommonWithEngine:sound.audioEngine
                                  path:sound.path
                              loadType:sound.loadType];

    if (!self) {
        return nil;
    }

    return self;
}

- (BOOL)backendPlayWithError:(RYError **)error {
    return YES;
}

- (BOOL)backendPauseWithError:(RYError **)error {
    return YES;
}

- (BOOL)backendStopWithError:(RYError **)error {
    return YES;
}

- (BOOL)backendSeekToTime:(double)time error:(RYError **)error {
    return YES;
}

- (BOOL)backendApplyVolume:(float)volume error:(RYError **)error {
    return YES;
}

- (BOOL)backendApplyLooping:(BOOL)looping error:(RYError **)error {
    return YES;
}

- (RYSoundPlaybackState)backendPlaybackState {
    return RYSoundPlaybackStateStopped;
}

- (double)backendDuration {
    return 1.0;
}

- (double)backendCurrentTime {
    return 0.0;
}

@end