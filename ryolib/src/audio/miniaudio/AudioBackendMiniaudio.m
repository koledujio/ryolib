#if defined(RYO_ENABLE_AUDIO_MINIAUDIO)

#import "AudioBackendMiniaudio.h"
#include "Miniaudio.h"
#include <stdlib.h>

typedef struct {
    ma_sound sound;
    BOOL paused;
} RYMiniaudioSoundHandle;

@implementation RYAudioBackendMiniaudio

- (instancetype)initBackendWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    self = [super init];
    if (!self) {
        return nil;
    }

    _engine = (ma_engine *)malloc(sizeof(ma_engine));
    if (!_engine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralOutOfMemory message:"Failed to allocate miniaudio engine memory"];
        [self release];
        return nil;
    }

    ma_result result = ma_engine_init(NULL, _engine);
    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendInitializationFailed message:ma_result_description(result)];
        free(_engine);
        _engine = NULL;
        [self release];
        return nil;
    }

    return self;
}

- (void)dealloc {
    if (_engine) {
        ma_engine_uninit(_engine);
        free(_engine);
        _engine = NULL;
    }

    [super dealloc];
}

- (BOOL)applyMasterVolume:(float)volume
                    error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_engine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Audio engine is not initialized"];
        return NO;
    }

    ma_result result = ma_engine_set_volume(_engine, volume);
    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(result)];
        return NO;
    }

    return YES;
}

- (void *)createSoundWithPath:(RYString *)path
                     loadType:(RYSoundLoadType)loadType
                        error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_engine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Miniaudio engine is not initialized"];
        return NULL;
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

    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)malloc(sizeof(RYMiniaudioSoundHandle));
    if (!handle) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralOutOfMemory message:"Failed to allocate sound memory"];
        return NULL;
    }

    ma_result result = ma_sound_init_from_file(
        _engine,
        path.UTF8String,
        flags,
        NULL,
        NULL,
        &handle->sound
    );

    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(result)];
        free(handle);
        return NULL;
    }

    handle->paused = NO;
    return handle;
}

- (void *)copySound:(void *)soundHandle
              error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_engine || !soundHandle) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Invalid source sound or engine"];
        return NULL;
    }

    RYMiniaudioSoundHandle *source = (RYMiniaudioSoundHandle *)soundHandle;
    RYMiniaudioSoundHandle *copy = (RYMiniaudioSoundHandle *)malloc(sizeof(RYMiniaudioSoundHandle));
    if (!copy) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralOutOfMemory message:"Failed to allocate sound copy memory"];
        return NULL;
    }

    ma_result result = ma_sound_init_copy(_engine, &source->sound, 0, NULL, &copy->sound);
    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(result)];
        free(copy);
        return NULL;
    }

    copy->paused = NO;
    return copy;
}

- (void)destroySound:(void *)soundHandle {
    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (handle) {
        ma_sound_uninit(&handle->sound);
        free(handle);
    }
}

- (BOOL)playSound:(void *)soundHandle
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    if (handle->paused) {
        ma_result startResult = ma_sound_start(&handle->sound);
        if (startResult != MA_SUCCESS) {
            if (error)
                *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(startResult)];
            return NO;
        }
        handle->paused = NO;
        return YES;
    }

    if (ma_sound_is_playing(&handle->sound)) {
        return YES;
    }

    ma_result seekResult = MA_SUCCESS;
    if (ma_sound_at_end(&handle->sound)) {
        seekResult = ma_sound_seek_to_pcm_frame(&handle->sound, 0);
    }

    if (seekResult != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(seekResult)];
        return NO;
    }

    ma_result startResult = ma_sound_start(&handle->sound);
    if (startResult != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(startResult)];
        return NO;
    }

    handle->paused = NO;
    return YES;
}

- (BOOL)pauseSound:(void *)soundHandle
             error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    ma_result result = ma_sound_stop(&handle->sound);
    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(result)];
        return NO;
    }

    handle->paused = YES;
    return YES;
}

- (BOOL)stopSound:(void *)soundHandle
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    if (![self pauseSound:soundHandle error:error] ||
        ![self seekSound:soundHandle toTime:0.0 error:error]) {
        return NO;
    }

    handle->paused = NO;
    return YES;
}

- (BOOL)seekSound:(void *)soundHandle
           toTime:(double)time
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle || !_engine) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    ma_uint32 sampleRate = time == 0.0 ? 0 : ma_engine_get_sample_rate(_engine);
    ma_uint64 seekFrame = (ma_uint64)(time * sampleRate);

    ma_result result = ma_sound_seek_to_pcm_frame(&handle->sound, seekFrame);
    if (result != MA_SUCCESS) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendError message:ma_result_description(result)];
        return NO;
    }

    return YES;
}

- (BOOL)setSound:(void *)soundHandle
          volume:(float)volume
           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    ma_sound_set_volume(&handle->sound, volume);
    return YES;
}

- (BOOL)setSound:(void *)soundHandle
         looping:(BOOL)looping
           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        if (error)
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralNotInitialized message:"Sound is not initialized"];
        return NO;
    }

    ma_sound_set_looping(&handle->sound, looping ? MA_TRUE : MA_FALSE);
    return YES;
}

- (RYSoundPlaybackState)soundPlaybackState:(void *)soundHandle {
    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        return RYSoundPlaybackStateStopped;
    }

    ma_bool32 playing = ma_sound_is_playing(&handle->sound);
    ma_bool32 atEnd = ma_sound_at_end(&handle->sound);

    if (playing) {
        return RYSoundPlaybackStatePlaying;
    } else if (atEnd) {
        return RYSoundPlaybackStateFinished;
    } else if (handle->paused) {
        return RYSoundPlaybackStatePaused;
    } else {
        return RYSoundPlaybackStateStopped;
    }
}

- (double)soundDuration:(void *)soundHandle {
    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        return 0.0;
    }

    float duration = 0.0f;
    ma_result result = ma_sound_get_length_in_seconds(&handle->sound, &duration);
    if (result != MA_SUCCESS) {
        return 0.0;
    }

    return (double)duration;
}

- (double)soundCurrentTime:(void *)soundHandle {
    RYMiniaudioSoundHandle *handle = (RYMiniaudioSoundHandle *)soundHandle;
    if (!handle) {
        return 0.0;
    }

    float cursor = 0.0f;
    ma_result result = ma_sound_get_cursor_in_seconds(&handle->sound, &cursor);
    if (result != MA_SUCCESS) {
        return 0.0;
    }

    return (double)cursor;
}

- (struct ma_engine *)miniaudioEngine {
    return _engine;
}

@end

#else

#import "AudioBackendMiniaudio.h"

@implementation RYAudioBackendMiniaudio

- (instancetype)initBackendWithError:(RYError **)error {
    if (error)
        *error = [[RYError alloc] initWithCode:RYErrorCodeAudioBackendPlatformNotSupported message:"Miniaudio backend is not supported on this platform"];
    
    return nil;
}

- (struct ma_engine *)miniaudioEngine {
    return NULL;
}

@end

#endif
