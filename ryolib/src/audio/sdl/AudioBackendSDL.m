#if defined(RYO_ENABLE_AUDIO_SDL)

#import "AudioBackendSDL.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>
#include <stdlib.h>

static unsigned int g_numAudioBackends = 0;

typedef struct {
    MIX_Track *track;
    MIX_Audio *audio;
    unsigned int *audioRefCount;
} RYSDLSoundHandle;

@implementation RYAudioBackendSDL

- (instancetype)initBackendWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    self = [super init];
    if (!self)
        return nil;

    _mixer = NULL;

    if (g_numAudioBackends == 0) {
        if (!MIX_Init()) {
            if (error)
                *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
            [self release];
            return nil;
        }
    }

    _mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!_mixer) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];

        if (g_numAudioBackends == 0) {
            MIX_Quit();
        }

        [self release];
        return nil;
    }

    g_numAudioBackends++;
    return self;
}

- (void)dealloc {
    if (_mixer) {
        MIX_DestroyMixer(_mixer);
        _mixer = NULL;

        if (g_numAudioBackends == 1) {
            MIX_Quit();
        }

        g_numAudioBackends--;
    }

    [super dealloc];
}

- (BOOL)applyMasterVolume:(float)volume
                    error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_mixer) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Audio engine is not initialized"];
        return NO;
    }

    if (!MIX_SetMixerGain(_mixer, volume)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
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

    if (!_mixer) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"SDL mixer is not initialized"];
        return NULL;
    }

    bool predecode = (loadType != RYSoundLoadTypeStream);
    MIX_Audio *audio = MIX_LoadAudio(_mixer, path.UTF8String, predecode);
    if (!audio) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NULL;
    }

    MIX_Track *track = MIX_CreateTrack(_mixer);
    if (!track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        MIX_DestroyAudio(audio);
        return NULL;
    }

    if (!MIX_SetTrackAudio(track, audio)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        MIX_DestroyTrack(track);
        MIX_DestroyAudio(audio);
        return NULL;
    }

    unsigned int *audioRefCount = (unsigned int *)malloc(sizeof(unsigned int));
    if (!audioRefCount) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Failed to allocate audio ref count"];
        MIX_DestroyTrack(track);
        MIX_DestroyAudio(audio);
        return NULL;
    }
    *audioRefCount = 1;

    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)malloc(sizeof(RYSDLSoundHandle));
    if (!handle) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Failed to allocate sound handle"];
        free(audioRefCount);
        MIX_DestroyTrack(track);
        MIX_DestroyAudio(audio);
        return NULL;
    }

    handle->track = track;
    handle->audio = audio;
    handle->audioRefCount = audioRefCount;
    return handle;
}

- (void *)copySound:(void *)soundHandle
              error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYSDLSoundHandle *source = (RYSDLSoundHandle *)soundHandle;
    if (!_mixer || !source || !source->audio) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid source sound or mixer"];
        return NULL;
    }

    MIX_Track *track = MIX_CreateTrack(_mixer);
    if (!track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NULL;
    }

    if (!MIX_SetTrackAudio(track, source->audio)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        MIX_DestroyTrack(track);
        return NULL;
    }

    if (source->audioRefCount) {
        (*source->audioRefCount)++;
    }

    RYSDLSoundHandle *copy = (RYSDLSoundHandle *)malloc(sizeof(RYSDLSoundHandle));
    if (!copy) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Failed to allocate copy sound handle"];
        if (source->audioRefCount) {
            (*source->audioRefCount)--;
        }
        MIX_DestroyTrack(track);
        return NULL;
    }

    copy->track = track;
    copy->audio = source->audio;
    copy->audioRefCount = source->audioRefCount;
    return copy;
}

- (void)destroySound:(void *)soundHandle {
    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (handle) {
        if (handle->track) {
            MIX_StopTrack(handle->track, 0);
            MIX_SetTrackAudio(handle->track, NULL);
            MIX_DestroyTrack(handle->track);
            handle->track = NULL;
        }

        if (handle->audioRefCount) {
            (*handle->audioRefCount)--;
            if (*handle->audioRefCount == 0) {
                if (handle->audio) {
                    MIX_DestroyAudio(handle->audio);
                    handle->audio = NULL;
                }
                free(handle->audioRefCount);
                handle->audioRefCount = NULL;
            }
        }

        free(handle);
    }
}

- (BOOL)playSound:(void *)soundHandle
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid sound track"];
        return NO;
    }

    if (MIX_TrackPaused(handle->track)) {
        if (!MIX_ResumeTrack(handle->track)) {
            if (error)
                *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
            return NO;
        }
        return YES;
    }

    if (MIX_TrackPlaying(handle->track)) {
        return YES;
    }

    MIX_SetTrackPlaybackPosition(handle->track, 0);

    if (!MIX_PlayTrack(handle->track, 0)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)pauseSound:(void *)soundHandle
             error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid sound track"];
        return NO;
    }

    if (!MIX_PauseTrack(handle->track)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)stopSound:(void *)soundHandle
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid sound track"];
        return NO;
    }

    if (!MIX_StopTrack(handle->track, 0)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    MIX_SetTrackPlaybackPosition(handle->track, 0);
    return YES;
}

- (BOOL)seekSound:(void *)soundHandle
           toTime:(double)time
            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid sound track"];
        return NO;
    }

    Sint64 ms = (Sint64)(time * 1000.0);
    Sint64 frames = MIX_TrackMSToFrames(handle->track, ms);
    if (frames < 0) {
        frames = 0;
    }

    if (!MIX_SetTrackPlaybackPosition(handle->track, frames)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
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

    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid sound track"];
        return NO;
    }

    if (!MIX_SetTrackGain(handle->track, volume)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)setSound:(void *)soundHandle
         looping:(BOOL)looping
           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid sound track"];
        return NO;
    }

    if (!MIX_SetTrackLoops(handle->track, looping ? -1 : 0)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (RYSoundPlaybackState)soundPlaybackState:(void *)soundHandle {
    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        return RYSoundPlaybackStateStopped;
    }

    if (MIX_TrackPlaying(handle->track)) {
        return RYSoundPlaybackStatePlaying;
    }
    if (MIX_TrackPaused(handle->track)) {
        return RYSoundPlaybackStatePaused;
    }
    if (handle->audio && MIX_GetTrackRemaining(handle->track) == 0 && MIX_GetTrackPlaybackPosition(handle->track) > 0) {
        return RYSoundPlaybackStateFinished;
    }
    return RYSoundPlaybackStateStopped;
}

- (double)soundDuration:(void *)soundHandle {
    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->audio) {
        return 0.0;
    }

    Sint64 frames = MIX_GetAudioDuration(handle->audio);
    if (frames < 0) {
        return 0.0;
    }

    Sint64 ms = MIX_AudioFramesToMS(handle->audio, frames);
    if (ms < 0) {
        return 0.0;
    }

    return (double)ms / 1000.0;
}

- (double)soundCurrentTime:(void *)soundHandle {
    RYSDLSoundHandle *handle = (RYSDLSoundHandle *)soundHandle;
    if (!handle || !handle->track) {
        return 0.0;
    }

    Sint64 frames = MIX_GetTrackPlaybackPosition(handle->track);
    if (frames < 0) {
        return 0.0;
    }

    Sint64 ms = MIX_TrackFramesToMS(handle->track, frames);
    if (ms < 0) {
        return 0.0;
    }

    return (double)ms / 1000.0;
}

- (struct MIX_Mixer *)sdlMixer {
    return _mixer;
}

@end

#else

#import "AudioBackendSDL.h"

@implementation RYAudioBackendSDL

- (instancetype)initBackendWithError:(RYError **)error {
    if (error)
        *error = [[RYError alloc] initWithCode:-1 message:"SDL audio backend is not supported on this platform"];
    
    return nil;
}

- (struct MIX_Mixer *)sdlMixer {
    return NULL;
}

@end

#endif
