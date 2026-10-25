#if defined(RYO_ENABLE_AUDIO_SDL)

#import "SoundInternal.h"
#import "AudioEngineInternal.h"

#include <stdlib.h>

@implementation RYSoundSDL

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

    RYAudioEngineSDL *sdlEngine = (RYAudioEngineSDL *)audioEngine;
    MIX_Mixer *mixer = sdlEngine.sdlMixer;
    if (!mixer) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid SDL audio engine"];
        [self release];
        return nil;
    }

    bool predecode = (loadType != RYSoundLoadTypeStream);
    _audio = MIX_LoadAudio(mixer, path.UTF8String, predecode);
    if (!_audio) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        [self release];
        return nil;
    }

    _track = MIX_CreateTrack(mixer);
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        MIX_DestroyAudio(_audio);
        _audio = NULL;
        [self release];
        return nil;
    }

    if (!MIX_SetTrackAudio(_track, _audio)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        MIX_DestroyTrack(_track);
        _track = NULL;
        MIX_DestroyAudio(_audio);
        _audio = NULL;
        [self release];
        return nil;
    }

    _audioRefCount = (unsigned int *)malloc(sizeof(unsigned int));
    if (!_audioRefCount) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Failed to allocate audio ref count"];
        MIX_DestroyTrack(_track);
        _track = NULL;
        MIX_DestroyAudio(_audio);
        _audio = NULL;
        [self release];
        return nil;
    }
    *_audioRefCount = 1;

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

    RYSoundSDL *sourceSound = (RYSoundSDL *)sound;
    RYAudioEngineSDL *sdlEngine = (RYAudioEngineSDL *)sound.audioEngine;
    MIX_Mixer *mixer = sdlEngine.sdlMixer;
    if (!mixer || !sourceSound.sdlAudio) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid source sound or audio engine"];
        [self release];
        return nil;
    }

    _track = MIX_CreateTrack(mixer);
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        [self release];
        return nil;
    }

    if (!MIX_SetTrackAudio(_track, sourceSound.sdlAudio)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        MIX_DestroyTrack(_track);
        _track = NULL;
        [self release];
        return nil;
    }

    _audio = sourceSound.sdlAudio;
    _audioRefCount = sourceSound.audioRefCount;
    if (_audioRefCount) {
        (*_audioRefCount)++;
    }

    _volume = sound.volume;
    _looping = sound.looping;
    MIX_SetTrackGain(_track, _volume);
    MIX_SetTrackLoops(_track, _looping ? -1 : 0);

    return self;
}

- (void)dealloc {
    if (_track) {
        MIX_StopTrack(_track, 0);
        MIX_SetTrackAudio(_track, NULL);
        MIX_DestroyTrack(_track);
        _track = NULL;
    }

    if (_audioRefCount) {
        (*_audioRefCount)--;
        if (*_audioRefCount == 0) {
            if (_audio) {
                MIX_DestroyAudio(_audio);
                _audio = NULL;
            }
            free(_audioRefCount);
            _audioRefCount = NULL;
        }
    }

    [super dealloc];
}

- (BOOL)backendPlayWithError:(RYError **)error {
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid track"];
        return NO;
    }

    if (MIX_TrackPaused(_track)) {
        if (!MIX_ResumeTrack(_track)) {
            if (error)
                *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
            return NO;
        }
        return YES;
    }

    if (MIX_TrackPlaying(_track)) {
        return YES;
    }

    MIX_SetTrackPlaybackPosition(_track, 0);

    if (!MIX_PlayTrack(_track, 0)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)backendPauseWithError:(RYError **)error {
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid track"];
        return NO;
    }

    if (!MIX_PauseTrack(_track)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)backendStopWithError:(RYError **)error {
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid track"];
        return NO;
    }

    if (!MIX_StopTrack(_track, 0)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    MIX_SetTrackPlaybackPosition(_track, 0);
    return YES;
}

- (BOOL)backendSeekToTime:(double)time error:(RYError **)error {
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid track"];
        return NO;
    }

    Sint64 ms = (Sint64)(time * 1000.0);
    Sint64 frames = MIX_TrackMSToFrames(_track, ms);
    if (frames < 0) {
        frames = 0;
    }

    if (!MIX_SetTrackPlaybackPosition(_track, frames)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)backendApplyVolume:(float)volume error:(RYError **)error {
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid track"];
        return NO;
    }

    if (!MIX_SetTrackGain(_track, volume)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)backendApplyLooping:(BOOL)looping error:(RYError **)error {
    if (!_track) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:"Invalid track"];
        return NO;
    }

    if (!MIX_SetTrackLoops(_track, looping ? -1 : 0)) {
        if (error)
            *error = [[RYError alloc] initWithCode:-1 message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (RYSoundPlaybackState)backendPlaybackState {
    if (!_track) {
        return RYSoundPlaybackStateStopped;
    }

    if (MIX_TrackPlaying(_track)) {
        return RYSoundPlaybackStatePlaying;
    }
    if (MIX_TrackPaused(_track)) {
        return RYSoundPlaybackStatePaused;
    }
    if (_audio && MIX_GetTrackRemaining(_track) == 0 && MIX_GetTrackPlaybackPosition(_track) > 0) {
        return RYSoundPlaybackStateFinished;
    }
    return RYSoundPlaybackStateStopped;
}

- (double)backendDuration {
    if (!_audio) {
        return 0.0;
    }

    Sint64 frames = MIX_GetAudioDuration(_audio);
    if (frames < 0) {
        return 0.0;
    }

    Sint64 ms = MIX_AudioFramesToMS(_audio, frames);
    if (ms < 0) {
        return 0.0;
    }

    return (double)ms / 1000.0;
}

- (double)backendCurrentTime {
    if (!_track) {
        return 0.0;
    }

    Sint64 frames = MIX_GetTrackPlaybackPosition(_track);
    if (frames < 0) {
        return 0.0;
    }

    Sint64 ms = MIX_TrackFramesToMS(_track, frames);
    if (ms < 0) {
        return 0.0;
    }

    return (double)ms / 1000.0;
}

- (MIX_Audio *)sdlAudio {
    return _audio;
}

- (MIX_Track *)sdlTrack {
    return _track;
}

- (unsigned int *)audioRefCount {
    return _audioRefCount;
}

@end

#else

#import "ryolib/audio/sdl/Sound.h"

@implementation RYSoundSDL

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
