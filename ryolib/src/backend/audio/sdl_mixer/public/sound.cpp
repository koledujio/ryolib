#include "../private.h"

#include "ryolib/audio.h"

#include <SDL3/SDL_error.h>

namespace ryo {
    Sound::Sound(const AudioEngine& engine, SoundDesc soundDesc) {
        RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");

        MIX_Audio* audio = MIX_LoadAudio(engine.internal->mixer, soundDesc.path, false);
        if (!audio) {
            error_set("Failed to load audio: %s", SDL_GetError());
            return;
        }

        MIX_Track* track = MIX_CreateTrack(engine.internal->mixer);
        if (!track) {
            error_set("Failed to create track for audio: %s", SDL_GetError());
            MIX_DestroyAudio(audio);
            return;
        }

        if (!MIX_SetTrackAudio(track, audio)) {
            error_set("Failed to set track audio: %s", SDL_GetError());
            MIX_DestroyTrack(track);
            MIX_DestroyAudio(audio);
            return;
        }

        InternalAudio* internAudio = new InternalAudio;
        internAudio->audio = audio;
        internAudio->refCount = 1;

        InternalSound* intern = new InternalSound;
        intern->audio = internAudio;
        intern->track = track;

        internal = intern;
    }

    Sound::Sound(const AudioEngine& engine, const Sound& source) {
        RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
        RYO_ENSURE_VOID(source.valid(), "Invalid source sound");

        MIX_Track* track = MIX_CreateTrack(engine.internal->mixer);
        if (!track) {
            error_set("Failed to create track for audio: %s", SDL_GetError());
            return;
        }

        if (!MIX_SetTrackAudio(track, source.internal->audio->audio)) {
            error_set("Failed to set track audio: %s", SDL_GetError());
            MIX_DestroyTrack(track);
            return;
        }

        source.internal->audio->refCount++;

        InternalSound* intern = new InternalSound;
        intern->audio = source.internal->audio;
        intern->track = track;

        internal = intern;
    }

    Sound::~Sound() {
        if (!internal)
            return;

        if (internal->track)
            MIX_DestroyTrack(internal->track);

        if (internal->audio) {
            internal->audio->refCount--;

            if (internal->audio->refCount == 0) {
                if (internal->audio->audio)
                    MIX_DestroyAudio(internal->audio->audio);
                
                delete internal->audio;
            }
        }

        delete internal;
    }

    Sound::Sound(Sound&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Sound& Sound::operator=(Sound&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->track)
                MIX_DestroyTrack(internal->track);

            if (internal->audio) {
                internal->audio->refCount--;

                if (internal->audio->refCount == 0) {
                    if (internal->audio->audio)
                        MIX_DestroyAudio(internal->audio->audio);

                    delete internal->audio;
                }
            }

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Sound::valid() const {
        return (internal != nullptr &&
                internal->track != nullptr &&
                internal->audio != nullptr &&
                internal->audio->refCount > 0 &&
                internal->audio->audio != nullptr);
    }

    bool Sound::play() {
        RYO_ENSURE(valid(), false, "Invalid sound");

        if (MIX_TrackPaused(internal->track)) {
            if (!MIX_ResumeTrack(internal->track)) {
                error_set("Failed to resume sound: %s", SDL_GetError());
                return false;
            }
        } else {
            if (!MIX_PlayTrack(internal->track, 0)) {
                error_set("Failed to play sound: %s", SDL_GetError());
                return false;
            }
        }

        return true;
    }

    bool Sound::pause() {
        RYO_ENSURE(valid(), false, "Invalid sound");

        if (!MIX_PauseTrack(internal->track)) {
            error_set("Failed to pause sound: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Sound::stop() {
        RYO_ENSURE(valid(), false, "Invalid sound");

        if (!MIX_StopTrack(internal->track, 0)) {
            error_set("Failed to stop sound: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void Sound::volume_set(float volume) {
        RYO_ENSURE_VOID(valid(), "Invalid sound");
        RYO_ENSURE_VOID(volume >= 0.0f, "Volume value is less than 0.0f");

        MIX_SetTrackGain(internal->track, volume);
    }

    void Sound::looping_set(bool looping) {
        RYO_ENSURE_VOID(valid(), "Invalid sound");
        
        MIX_SetTrackLoops(internal->track, looping ? -1 : 0);
    }

    uint32_t Sound::duration_miliseconds_get() const {
        RYO_ENSURE(valid(), 0, "Invalid sound");

        return (uint32_t)MIX_AudioFramesToMS(internal->audio->audio, MIX_GetAudioDuration(internal->audio->audio));
    }

    float Sound::duration_seconds_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid sound");

        return (float)duration_miliseconds_get() / 1000.0f;
    }

    uint32_t Sound::current_miliseconds_get() const {
        RYO_ENSURE(valid(), 0, "Invalid sound");

        return (uint32_t)MIX_AudioFramesToMS(internal->audio->audio, MIX_GetTrackPlaybackPosition(internal->track));
    }

    float Sound::current_seconds_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid sound");

        return (float)current_miliseconds_get() / 1000.0f;
    }
}