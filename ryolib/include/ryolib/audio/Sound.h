#pragma once

#import "../objcpp/RYObject.h"
#import "../objcpp/RYString.h"
#import "../objcpp/RYError.h"

#import "AudioEngine.h"

typedef enum {
    RYSoundLoadTypeDecode,
    RYSoundLoadTypeStream
} RYSoundLoadType;

typedef enum {
    RYSoundPlaybackStatePlaying,
    RYSoundPlaybackStatePaused,
    RYSoundPlaybackStateStopped,
    RYSoundPlaybackStateFinished
} RYSoundPlaybackState;

@interface RYSound : RYObject {
@protected
    RYAudioEngine *_audioEngine;
    RYString *_path;
    RYSoundLoadType _loadType;

    float _volume;
    BOOL _looping;
}

@property (nonatomic, readonly) RYAudioEngine *audioEngine;
@property (nonatomic, readonly) RYString *path;
@property (nonatomic, readonly) RYSoundLoadType loadType;

@property (nonatomic, readonly) float volume;
@property (nonatomic, readonly) BOOL looping;
@property (nonatomic, readonly) RYSoundPlaybackState playbackState;
@property (nonatomic, readonly) double duration;
@property (nonatomic, readonly) double currentTime;

+ (instancetype)soundWithEngine:(RYAudioEngine *)audioEngine
                           path:(RYString *)path
                       loadType:(RYSoundLoadType)loadType
                          error:(RYError **)error;

+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine
                           path:(RYString *)path
                      loadType:(RYSoundLoadType)loadType
                         error:(RYError **)error;

- (instancetype)copyWithError:(RYError **)error;

- (BOOL)playWithError:(RYError **)error;
- (BOOL)pauseWithError:(RYError **)error;
- (BOOL)stopWithError:(RYError **)error;
- (BOOL)seekToTime:(double)time error:(RYError **)error;

- (BOOL)setVolume:(float)volume error:(RYError **)error;
- (BOOL)setLooping:(BOOL)looping error:(RYError **)error;

@end