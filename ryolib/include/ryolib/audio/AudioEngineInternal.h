#pragma once

#import "AudioEngine.h"

@interface RYAudioEngine ()

- (instancetype)initCommon;

- (instancetype)initBackendWithError:(RYError **)error;

- (BOOL)applyBackendMasterVolume:(float)volume
                           error:(RYError **)error;

@end