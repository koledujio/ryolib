#pragma once

#import "../objcpp/RYObject.h"
#import "../objcpp/RYString.h"
#import "../objcpp/RYError.h"
#import "AudioEngine.h"
#import "AudioBackend.h"

/**
 * Represents a sound loaded by an audio engine.
 * By default the load type is RYSoundLoadTypeDecode,
 * volume is 1.0f and looping is disabled.
 */
@interface RYSound : RYObject {
@protected
    RYAudioEngine *_audioEngine;
    RYString *_path;
    RYSoundLoadType _loadType;

    float _volume;
    BOOL _looping;

    void *_backendSound;
}

@property (nonatomic, readonly) RYAudioEngine *audioEngine;
@property (nonatomic, readonly) RYString *path;
@property (nonatomic, readonly) RYSoundLoadType loadType;

@property (nonatomic, readonly) float volume;
@property (nonatomic, readonly) BOOL looping;
@property (nonatomic, readonly) RYSoundPlaybackState playbackState;
@property (nonatomic, readonly) double duration;
@property (nonatomic, readonly) double currentTime;
@property (nonatomic, readonly) void *backendSound;

/** Creates a new sound with the default load type. */
+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine
                              path:(RYString *)path;

/** Creates a new sound with the specified load type. */
+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine
                              path:(RYString *)path
                          loadType:(RYSoundLoadType)loadType;

/** Creates a new sound with the default load type, reports errors. */
+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine
                              path:(RYString *)path
                             error:(RYError **)error;

/** Creates a new sound with the specified load type, reports errors. */
+ (instancetype)newSoundWithEngine:(RYAudioEngine *)audioEngine
                              path:(RYString *)path
                          loadType:(RYSoundLoadType)loadType
                             error:(RYError **)error;

- (instancetype)initWithEngine:(RYAudioEngine *)audioEngine
                          path:(RYString *)path
                      loadType:(RYSoundLoadType)loadType
                  backendSound:(void *)backendSound;

/**
 * Creates an independent sound instance sharing the original's underlying
 * audio resources. The copy has independent playback state and settings.
 */
- (instancetype)copySound;
/**
 * Creates an independent sound instance sharing the original's underlying
 * audio resources. Reports errors.
 */
- (instancetype)copyWithError:(RYError **)error;

/** Plays the sound from the beginning if stopped or finished or resumes playback if paused. */
- (BOOL)play;
/** Plays the sound from the beginning if stopped or finished or resumes playback if paused, reports errors. */
- (BOOL)playWithError:(RYError **)error;

- (BOOL)pause;
- (BOOL)pauseWithError:(RYError **)error;

/** Stops playback and resets the playback position to the beginning. */
- (BOOL)stop;
/** Stops playback and resets the playback position to the beginning, reports errors. */
- (BOOL)stopWithError:(RYError **)error;

/** 
 * Seeks to the specified time in seconds.
 * @param time Target time, must be finite and non-negative.
 */
- (BOOL)seekToTime:(double)time;
/**
 * Seeks to the specified time in seconds, reports errors.
 * @param time Target time, must be finite and non-negative.
 */
- (BOOL)seekToTime:(double)time error:(RYError **)error;

/**
 * Sets the sound volume.
 * @param volume Target volume, must be finite and non-negative.
 */
- (BOOL)setVolume:(float)volume;
/**
 * Sets the sound volume, reports errors.
 * @param volume Target volume, must be finite and non-negative.
 */
- (BOOL)setVolume:(float)volume error:(RYError **)error;

- (BOOL)setLooping:(BOOL)looping;
- (BOOL)setLooping:(BOOL)looping error:(RYError **)error;

@end
