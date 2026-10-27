#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYString.h"
#import "../objc/RYError.h"

/** Determines how a sound is loaded. */
typedef enum {
    RYSoundLoadTypeDecode,
    RYSoundLoadTypeStream
} RYSoundLoadType;

/**
 * Describes a sound's playback state.
 * Stopped indicates playback has been stopped explicitly.
 * Finished indicates playback has reached the end naturally.
 */
typedef enum {
    RYSoundPlaybackStatePlaying,
    RYSoundPlaybackStatePaused,
    RYSoundPlaybackStateStopped,
    RYSoundPlaybackStateFinished
} RYSoundPlaybackState;

/**
 * Base class for audio backends.
 * 
 * Subclasses implement audio operations for a specific audio system.
 */
@interface RYAudioBackend : RYObject

- (instancetype)initBackendWithError:(RYError **)error;

/**
 * Applies the engine's master volume.
 * @param volume Target volume, must be finite and non-negative.
 */
- (BOOL)applyMasterVolume:(float)volume
                    error:(RYError **)error;

/**
 * Creates a sound and returns its backend handle.
 * The handle is owned by the backend and must be released with destroySound:.
 */
- (void *)createSoundWithPath:(RYString *)path
                     loadType:(RYSoundLoadType)loadType
                        error:(RYError **)error;

/**
 * Creates a copy of an existing sound and returns its backend handle.
 * The handle is owned by the backend and must be released with destroySound:.
 */
- (void *)copySound:(void *)soundHandle
              error:(RYError **)error;

/** Destroys a sound handle created by this backend. */
- (void)destroySound:(void *)soundHandle;

/** If stopped or finished, starts playback from beginning, if paused, resumes playback. */
- (BOOL)playSound:(void *)soundHandle
            error:(RYError **)error;

- (BOOL)pauseSound:(void *)soundHandle
             error:(RYError **)error;

- (BOOL)stopSound:(void *)soundHandle
            error:(RYError **)error;

/**
 * Seeks to the specified time.
 * @param time Target time, in seconds, must be finite and non-negative.
 */
- (BOOL)seekSound:(void *)soundHandle
           toTime:(double)time
            error:(RYError **)error;

/**
 * Sets a sound's playback volume.
 * @param volume Target volume, must be finite and non-negative.
 */
- (BOOL)setSound:(void *)soundHandle
          volume:(float)volume
           error:(RYError **)error;

- (BOOL)setSound:(void *)soundHandle
         looping:(BOOL)looping
           error:(RYError **)error;

- (RYSoundPlaybackState)soundPlaybackState:(void *)soundHandle;

- (double)soundDuration:(void *)soundHandle;

- (double)soundCurrentTime:(void *)soundHandle;

@end
