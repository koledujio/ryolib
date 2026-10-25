#import "ryolib/audio/SoundInternal.h"

#import "ryolib/audio/stub/Sound.h"
#import "ryolib/audio/miniaudio/Sound.h"
#import "ryolib/audio/sdl/Sound.h"

@implementation RYSound

@synthesize audioEngine = _audioEngine;
@synthesize path = _path;
@synthesize loadType = _loadType;

@synthesize volume = _volume;
@synthesize looping = _looping;

+ (instancetype)soundWithEngine:(RYAudioEngine *)audioEngine
                           path:(RYString *)path
                       loadType:(RYSoundLoadType)loadType
                          error:(RYError **)error {
    return [self newSoundWithEngine:audioEngine path:path loadType:loadType error:error];
}

+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine
                              path:(RYString *)path
                          loadType:(RYSoundLoadType)loadType
                             error:(RYError **)error {
    if (!audioEngine) {
        if (error) {
            *error = [[RYError alloc] initWithCode:-1 message:"Audio engine is nil"];
        }
        return nil;
    }

    if (!path) {
        if (error) {
            *error = [[RYError alloc] initWithCode:-1 message:"Sound path is nil"];
        }
        return nil;
    }
    
    Class soundClass = Nil;

    switch (audioEngine.backend) {
        case RYAudioEngineBackendMiniaudio:
            soundClass = [RYSoundMiniaudio class];
            break;
        
        case RYAudioEngineBackendSDL3:
            soundClass = [RYSoundSDL class];
            break;

        case RYAudioEngineBackendStub:
            soundClass = [RYSoundStub class];
            break;
        
        default:
            break;
    }

    if (soundClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid audio backend"];
        }

        return nil;
    }

    return [[soundClass alloc] backendInitWithEngine:(RYAudioEngine *)audioEngine
                                           path:(RYString *)path
                                       loadType:(RYSoundLoadType)loadType
                                          error:error];
}

- (instancetype)copyWithError:(RYError **)error {
    Class soundClass = Nil;

    switch (_audioEngine.backend) {
        case RYAudioEngineBackendMiniaudio:
            soundClass = [RYSoundMiniaudio class];
            break;
        
        case RYAudioEngineBackendSDL3:
            soundClass = [RYSoundSDL class];
            break;

        case RYAudioEngineBackendStub:
            soundClass = [RYSoundStub class];
            break;
        
        default:
            break;
    }

    if (soundClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid audio backend"];
        }

        return nil;
    }

    return [[soundClass alloc] backendInitCopyOfSound:self
                                           error:error];
}

- (void)dealloc {
    [_audioEngine release];
    [_path release];

    [super dealloc];
}

- (BOOL)playWithError:(RYError **)error {
    return [self backendPlayWithError:error];
}

- (BOOL)pauseWithError:(RYError **)error {
    return [self backendPauseWithError:error];
}

- (BOOL)stopWithError:(RYError **)error {
    return [self backendStopWithError:error];
}

- (BOOL)seekToTime:(double)time error:(RYError **)error {
    if (time < 0.0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid time (below 0.0)"];
        }

        return NO;
    }

    if (![self backendSeekToTime:time error:error]) {
        return NO;
    }

    return YES;
}

- (BOOL)setVolume:(float)volume error:(RYError **)error {
    if (volume < 0.0f) {
        if (error) {
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid volume amount (below 0.0f)"];
        }

        return NO;
    }

    if (![self backendApplyVolume:volume error:error]) {
        return NO;
    }

    _volume = volume;
    return YES;
}

- (BOOL)setLooping:(BOOL)looping error:(RYError **)error {
    if (![self backendApplyLooping:looping error:error]) {
        return NO;
    }

    _looping = looping;
    return YES;
}

- (RYSoundPlaybackState)playbackState {
    return [self backendPlaybackState];
}

- (double)duration {
    return [self backendDuration];
}

- (double)currentTime {
    return [self backendCurrentTime];
}

- (instancetype)initCommonWithEngine:(RYAudioEngine *)audioEngine
                                path:(RYString *)path
                            loadType:(RYSoundLoadType)loadType {
    self = [super init];

    if (!self) {
        return nil;
    }

    _audioEngine = [audioEngine retain];
    _path = [path retain];
    _loadType = loadType;

    _volume = 1.0f;
    _looping = NO;

    return self;
}

- (instancetype)backendInitWithEngine:(RYAudioEngine *)audioEngine
                                 path:(RYString *)path
                             loadType:(RYSoundLoadType)loadType
                                error:(RYError **)error {
    return nil;
}

- (instancetype)backendInitCopyOfSound:(RYSound *)sound
                                 error:(RYError **)error {
    return nil;
}

- (BOOL)backendPlayWithError:(RYError **)error {
    return NO;
}

- (BOOL)backendPauseWithError:(RYError **)error {
    return NO;
}

- (BOOL)backendStopWithError:(RYError **)error {
    return NO;
}

- (BOOL)backendSeekToTime:(double)time error:(RYError **)error {
    return NO;
}

- (BOOL)backendApplyVolume:(float)volume error:(RYError **)error {
    return NO;
}

- (BOOL)backendApplyLooping:(BOOL)looping error:(RYError **)error {
    return NO;
}

- (RYSoundPlaybackState)backendPlaybackState {
    return RYSoundPlaybackStateStopped;
}

- (double)backendDuration {
    return 0.0;
}

- (double)backendCurrentTime {
    return 0.0;
}

@end