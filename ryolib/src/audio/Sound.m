#import "ryolib/audio/Sound.h"

@implementation RYSound

@synthesize audioEngine = _audioEngine;
@synthesize path = _path;
@synthesize loadType = _loadType;
@synthesize volume = _volume;
@synthesize looping = _looping;
@synthesize backendSound = _backendSound;

+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine path:(RYString *)path {
    return [self newSoundWithEngine:audioEngine path:path loadType:RYSoundLoadTypeDecode error:NULL];
}

+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine path:(RYString *)path loadType:(RYSoundLoadType)loadType {
    return [self newSoundWithEngine:audioEngine path:path loadType:loadType error:NULL];
}

+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine path:(RYString *)path error:(RYError **)error {
    return [self newSoundWithEngine:audioEngine path:path loadType:RYSoundLoadTypeDecode error:error];
}

+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine
                              path:(RYString *)path
                          loadType:(RYSoundLoadType)loadType
                             error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioInvalidAudioEngine message:"Audio engine is nil"];
        return nil;
    }

    if (!path) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioInvalidSoundPath message:"Sound path is nil"];
        return nil;
    }

    void *backendSound = [audioEngine.backendInstance createSoundWithPath:path loadType:loadType error:error];
    if (!backendSound) {
        return nil;
    }

    RYSound *sound = [[RYSound alloc] initWithEngine:audioEngine
                                                path:path
                                            loadType:loadType
                                        backendSound:backendSound];
    return sound;
}

- (instancetype)initWithEngine:(RYAudioEngine *)audioEngine
                          path:(RYString *)path
                      loadType:(RYSoundLoadType)loadType
                  backendSound:(void *)backendSound {
    self = [super init];
    if (!self) {
        return nil;
    }

    _audioEngine = [audioEngine retain];
    _path = [path retain];
    _loadType = loadType;
    _volume = 1.0f;
    _looping = NO;
    _backendSound = backendSound;

    return self;
}

- (instancetype)copySound {
    return [self copyWithError:NULL];
}

- (instancetype)copyWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_backendSound || !_audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return nil;
    }

    void *copiedHandle = [_audioEngine.backendInstance copySound:_backendSound error:error];
    if (!copiedHandle) {
        return nil;
    }

    RYSound *copy = [[RYSound alloc] initWithEngine:_audioEngine
                                               path:_path
                                           loadType:_loadType
                                       backendSound:copiedHandle];
    if (copy) {
        [copy setVolume:_volume error:NULL];
        [copy setLooping:_looping error:NULL];
    }
    return copy;
}

- (void)dealloc {
    if (_backendSound && _audioEngine) {
        [_audioEngine.backendInstance destroySound:_backendSound];
        _backendSound = NULL;
    }

    [_audioEngine release];
    [_path release];

    [super dealloc];
}

- (BOOL)play {
    return [self playWithError:NULL];
}

- (BOOL)playWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_backendSound || !_audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    return [_audioEngine.backendInstance playSound:_backendSound error:error];
}

- (BOOL)pause {
    return [self pauseWithError:NULL];
}

- (BOOL)pauseWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_backendSound || !_audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    return [_audioEngine.backendInstance pauseSound:_backendSound error:error];
}

- (BOOL)stop {
    return [self stopWithError:NULL];
}

- (BOOL)stopWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_backendSound || !_audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    return [_audioEngine.backendInstance stopSound:_backendSound error:error];
}

- (BOOL)seekToTime:(double)time {
    return [self seekToTime:time error:NULL];
}

- (BOOL)seekToTime:(double)time error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (time < 0.0) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioInvalidSeekTime message:"Invalid time (below 0.0)"];
        return NO;
    }

    if (!_backendSound || !_audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    return [_audioEngine.backendInstance seekSound:_backendSound toTime:time error:error];
}

- (BOOL)setVolume:(float)volume {
    return [self setVolume:volume error:NULL];
}

- (BOOL)setVolume:(float)volume error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (volume < 0.0f) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioInvalidVolume message:"Invalid volume amount (below 0.0f)"];
        return NO;
    }

    if (!_backendSound || !_audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    if (![_audioEngine.backendInstance setSound:_backendSound volume:volume error:error]) {
        return NO;
    }

    _volume = volume;
    return YES;
}

- (BOOL)setLooping:(BOOL)looping {
    return [self setLooping:looping error:NULL];
}

- (BOOL)setLooping:(BOOL)looping error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_backendSound || !_audioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    if (![_audioEngine.backendInstance setSound:_backendSound looping:looping error:error]) {
        return NO;
    }

    _looping = looping;
    return YES;
}

- (RYSoundPlaybackState)playbackState {
    if (!_backendSound || !_audioEngine) {
        return RYSoundPlaybackStateStopped;
    }
    return [_audioEngine.backendInstance soundPlaybackState:_backendSound];
}

- (double)duration {
    if (!_backendSound || !_audioEngine) {
        return 0.0;
    }
    return [_audioEngine.backendInstance soundDuration:_backendSound];
}

- (double)currentTime {
    if (!_backendSound || !_audioEngine) {
        return 0.0;
    }
    return [_audioEngine.backendInstance soundCurrentTime:_backendSound];
}

@end
