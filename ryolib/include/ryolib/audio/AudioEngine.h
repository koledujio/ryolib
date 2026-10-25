#pragma once

#import "../objcpp/RYObject.h"
#import "../objcpp/RYError.h"

typedef enum {
    RYAudioEngineBackendDefault,
    RYAudioEngineBackendStub,
    RYAudioEngineBackendMiniaudio,
    RYAudioEngineBackendSDL3
} RYAudioEngineBackend;

@interface RYAudioEngine : RYObject {
@protected
    RYAudioEngineBackend _backend;
    float _masterVolume;
}

@property (nonatomic, readonly) RYAudioEngineBackend backend;
@property (nonatomic, readonly) float masterVolume;

+ (instancetype)engineWithBackend:(RYAudioEngineBackend)backend
                            error:(RYError **)error;

+ (instancetype)newEngineWithBackend:(RYAudioEngineBackend)backend
                               error:(RYError **)error;

- (BOOL)setMasterVolume:(float)masterVolume
                  error:(RYError **)error;

@end