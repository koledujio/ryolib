#pragma once

#import "Sound.h"

@interface RYSound ()

- (instancetype)initCommonWithEngine:(RYAudioEngine *)audioEngine
                                path:(RYString *)path
                            loadType:(RYSoundLoadType)loadType;

- (instancetype)backendInitWithEngine:(RYAudioEngine *)audioEngine
                                 path:(RYString *)path
                             loadType:(RYSoundLoadType)loadType
                                error:(RYError **)error;

- (instancetype)backendInitCopyOfSound:(RYSound *)sound
                                 error:(RYError **)error;

- (BOOL)backendPlayWithError:(RYError **)error;
- (BOOL)backendPauseWithError:(RYError **)error;
- (BOOL)backendStopWithError:(RYError **)error;
- (BOOL)backendSeekToTime:(double)time error:(RYError **)error;

- (BOOL)backendApplyVolume:(float)volume error:(RYError **)error;
- (BOOL)backendApplyLooping:(BOOL)looping error:(RYError **)error;

- (RYSoundPlaybackState)backendPlaybackState;
- (double)backendDuration;
- (double)backendCurrentTime;

@end