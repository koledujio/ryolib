#pragma once

#import "../objcpp/RYObject.h"
#import "../objcpp/RYError.h"
#import "AudioBackend.h"

/**
 * Identifies the audio backend used by an audio engine.
 */
typedef enum {
    RYAudioEngineBackendDefault,
    RYAudioEngineBackendStub,
    RYAudioEngineBackendMiniaudio,
    RYAudioEngineBackendSDL3,
    RYAudioEngineBackendCustom
} RYAudioEngineBackend;

/**
 * @brief Manages audio playback and its underlying audio backend.
 * 
 * An audio engine owns its backend instance and provides controls
 * shared by the sounds created with it.
 * The default master volume is 1.0f;
 */
@interface RYAudioEngine : RYObject {
@protected
    RYAudioEngineBackend _backend;
    float _masterVolume;
    RYAudioBackend *_backendInstance;
}

@property (nonatomic, readonly) RYAudioEngineBackend backend;
@property (nonatomic, readonly) float masterVolume;
@property (nonatomic, readonly) RYAudioBackend *backendInstance;

+ (instancetype)newEngineWithBackend:(RYAudioEngineBackend)backend;
+ (instancetype)newEngineWithBackend:(RYAudioEngineBackend)backend error:(RYError **)error;

/** Creates a new audio engine, which retains and uses the specified custom audio backend. */
+ (instancetype)newEngineWithCustomBackend:(RYAudioBackend *)customBackend;
/** Creates a new audio engine, which retains and uses the specified custom audio backend, reports errors. */
+ (instancetype)newEngineWithCustomBackend:(RYAudioBackend *)customBackend error:(RYError **)error;

- (instancetype)initWithBackendInstance:(RYAudioBackend *)backendInstance
                            backendType:(RYAudioEngineBackend)backend;

/**
 * @brief Sets the master volume.
 * @param masterVolume Target volume value, must be finite and non-negative.
 */
- (BOOL)setMasterVolume:(float)masterVolume;
/**
 * Sets the master volume and reports errors.
 * @param masterVolume Target volume value, must be finite and non-negative.
 */
- (BOOL)setMasterVolume:(float)masterVolume error:(RYError **)error;

@end
