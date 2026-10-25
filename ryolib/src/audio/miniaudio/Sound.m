#if defined(RYO_ENABLE_AUDIO_MINIAUDIO)

#import "ryolib/audio/miniaudio/Sound.h"
#import "SoundInternal.h"
#import "AudioEngineInternal.h"

#include <stdlib.h>

@implementation RYSoundMiniaudio

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

    RYAudioEngineMiniaudio *maEngine = (RYAudioEngineMiniaudio *)audioEngine;
    if (!maEngine.miniaudioEngine) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid miniaudio engine"];
        [self release];
        return nil;
    }

    ma_uint32 flags = 0;
    switch (loadType) {
        case RYSoundLoadTypeDecode:
            flags |= MA_SOUND_FLAG_DECODE;
            break;
        
        case RYSoundLoadTypeStream:
            flags |= MA_SOUND_FLAG_STREAM;
            break;
        
        default:
            break;
    }

    _sound = malloc(sizeof(ma_sound));
    if (!_sound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Failed to allocate sound memory"];
        [self release];
        return nil;
    }

    ma_result result = ma_sound_init_from_file(
        maEngine.miniaudioEngine,
        path.UTF8String,
        flags,
        NULL,
        NULL,
        _sound
    );

    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(result)];
        free(_sound);
        _sound = NULL;
        [self release];
        return nil;
    }

    _paused = NO;

    return self;
}

- (instancetype)backendInitCopyOfSound:(RYSound *)sound
                                 error:(RYError **)error {
    if (!sound) {
        return nil;
    }

    self = [super initCommonWithEngine:sound.audioEngine
                                  path:sound.path
                              loadType:sound.loadType];

    if (!self) {
        return nil;
    }

    RYSoundMiniaudio *sourceSound = (RYSoundMiniaudio *)sound;
    RYAudioEngineMiniaudio *maEngine = (RYAudioEngineMiniaudio *)sound.audioEngine;

    ma_sound *sourceMaSound = sourceSound.miniaudioSound;
    if (!maEngine.miniaudioEngine || !sourceMaSound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid source sound or miniaudio engine"];
        [self release];
        return nil;
    }

    _sound = malloc(sizeof(ma_sound));
    if (!_sound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Failed to allocate sound memory"];
        [self release];
        return nil;
    }

    ma_result result = ma_sound_init_copy(
        maEngine.miniaudioEngine,
        sourceMaSound,
        0,
        NULL,
        _sound
    );

    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(result)];
        free(_sound);
        _sound = NULL;
        [self release];
        return nil;
    }

    _volume = sound.volume;
    _looping = sound.looping;
    _paused = NO;
    ma_sound_set_volume(_sound, _volume);
    ma_sound_set_looping(_sound, _looping ? MA_TRUE : MA_FALSE);

    return self;
}

- (void)dealloc {
    if (_sound) {
        ma_sound_uninit(_sound);
        free(_sound);
        _sound = NULL;
    }

    [super dealloc];
}

- (BOOL)backendPlayWithError:(RYError **)error {
    if (!_sound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Sound is not initialized"];
        return NO;
    }

    ma_result seekResult = MA_SUCCESS;
    if (ma_sound_at_end(_sound)) {
        seekResult = ma_sound_seek_to_pcm_frame(_sound, 0);
    }

    if (seekResult != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(seekResult)];
        return NO;
    }

    ma_result startResult = ma_sound_start(_sound);
    
    if (startResult != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(startResult)];
        return NO;
    }

    _paused = NO;

    return YES;
}

- (BOOL)backendPauseWithError:(RYError **)error {
    if (!_sound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Sound is not initialized"];
        return NO;
    }

    ma_result result = ma_sound_stop(_sound);

    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(result)];
        return NO;
    }

    _paused = YES;

    return YES;
}

- (BOOL)backendStopWithError:(RYError **)error {
    if (![self backendPauseWithError:error] ||
        ![self backendSeekToTime:0.0 error:error]) {
        return NO;
    }

    _paused = NO;

    return YES;
}

- (BOOL)backendSeekToTime:(double)time error:(RYError **)error {
    if (!_sound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Sound is not initialized"];
        return NO;
    }

    RYAudioEngineMiniaudio *maEngine = (RYAudioEngineMiniaudio *)_audioEngine;
    ma_uint32 sampleRate = time == 0.0 ? 0 : ma_engine_get_sample_rate(maEngine.miniaudioEngine);
    ma_uint64 seekFrame = (ma_uint64)(time * sampleRate);

    ma_result result = ma_sound_seek_to_pcm_frame(_sound, seekFrame);

    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:ma_result_description(result)];
        return NO;
    }

    return YES;
}

- (BOOL)backendApplyVolume:(float)volume error:(RYError **)error {
    if (!_sound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Sound is not initialized"];
        return NO;
    }

    ma_sound_set_volume(_sound, volume);
    return YES;
}

- (BOOL)backendApplyLooping:(BOOL)looping error:(RYError **)error {
    if (!_sound) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Sound is not initialized"];
        return NO;
    }

    ma_sound_set_looping(_sound, looping ? MA_TRUE : MA_FALSE);
    return YES;
}

- (RYSoundPlaybackState)backendPlaybackState {
    if (!_sound) {
        return RYSoundPlaybackStateStopped;
    }

    ma_bool32 playing = ma_sound_is_playing(_sound);
    ma_bool32 atEnd = ma_sound_at_end(_sound);

    if (playing) {
        return RYSoundPlaybackStatePlaying;
    } else if (atEnd) {
        return RYSoundPlaybackStateFinished;
    } else if (_paused) {
        return RYSoundPlaybackStatePaused;
    } else {
        return RYSoundPlaybackStateStopped;
    }
}

- (double)backendDuration {
    if (!_sound) {
        return 0.0;
    }

    float duration;
    
    ma_result result = ma_sound_get_length_in_seconds(_sound, &duration);

    if (result != MA_SUCCESS) {
        return 0.0;
    }

    return (double)duration;
}

- (double)backendCurrentTime {
    if (!_sound) {
        return 0.0;
    }

    float cursor = 0.0f;

    ma_result result = ma_sound_get_cursor_in_seconds(_sound, &cursor);

    if (result != MA_SUCCESS) {
        return 0.0;
    }

    return (double)cursor;
}

- (ma_sound *)miniaudioSound {
    return _sound;
}

@end

#else

#import "ryolib/audio/miniaudio/Sound.h"

@implementation RYSoundMiniaudio

- (instancetype)backendInitWithEngine:(RYAudioEngine *)audioEngine
                                 path:(RYString *)path
                             loadType:(RYSoundLoadType)loadType
                                error:(RYError **)error {
    if (error)
        *error = [[RYError alloc] initWithCode:-1 message:"Unsupported platform"];

    return nil;
}

@end

#endif
