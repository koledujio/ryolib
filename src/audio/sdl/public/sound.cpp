#include "../AudioBackendSDL.h"

#include <SDL3/SDL_error.h>

namespace ryo {
    namespace sdl_mixer_audio {
        SdlMixerSound::SdlMixerSound(SdlMixerBackend& engine, const char* path, SoundLoadType loadType) {
            RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
            RYO_ENSURE_VOID(path != nullptr, "Sound path is nullptr");

            bool predecode = (loadType != SOUND_LOAD_TYPE_STREAM);

            MIX_Audio* loadedAudio = MIX_LoadAudio(engine.mixer, path, predecode);
            if (!loadedAudio) {
                error_set("Failed to load audio: %s", SDL_GetError());
                return;
            }

            track = MIX_CreateTrack(engine.mixer);
            if (!track) {
                error_set("Failed to create track for audio: %s", SDL_GetError());
                MIX_DestroyAudio(loadedAudio);
                return;
            }

            if (!MIX_SetTrackAudio(track, loadedAudio)) {
                error_set("Failed to set track audio: %s", SDL_GetError());
                MIX_DestroyTrack(track);
                MIX_DestroyAudio(loadedAudio);
                track = nullptr;
                return;
            }

            audio = new SdlMixerAudio;
            audio->audio = loadedAudio;
            audio->refCount = 1;
        }

        SdlMixerSound::SdlMixerSound(SdlMixerBackend& engine, const SdlMixerSound& source) {
            RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
            RYO_ENSURE_VOID(source.valid(), "Invalid source sound");

            track = MIX_CreateTrack(engine.mixer);
            if (!track) {
                error_set("Failed to create track for audio: %s", SDL_GetError());
                return;
            }

            if (!MIX_SetTrackAudio(track, source.audio->audio)) {
                error_set("Failed to set track audio: %s", SDL_GetError());
                MIX_DestroyTrack(track);
                track = nullptr;
                return;
            }

            source.audio->refCount++;
            audio = source.audio;
        }

        SdlMixerSound::~SdlMixerSound() {
            if (track) {
                MIX_StopTrack(track, 0);
                MIX_SetTrackAudio(track, nullptr);
                MIX_DestroyTrack(track);
                track = nullptr;
            }

            if (audio) {
                audio->refCount--;

                if (audio->refCount == 0) {
                    if (audio->audio) {
                        MIX_DestroyAudio(audio->audio);
                        audio->audio = nullptr;
                    }

                    delete audio;
                }

                audio = nullptr;
            }
        }

        bool SdlMixerSound::valid() const {
            return audio != nullptr &&
                   track != nullptr &&
                   audio->refCount > 0 &&
                   audio->audio != nullptr;
        }

        bool SdlMixerSound::play() {
            RYO_ENSURE(valid(), false, "Invalid sound");

            if (MIX_TrackPaused(track)) {
                if (!MIX_ResumeTrack(track)) {
                    error_set("Failed to resume sound: %s", SDL_GetError());
                    return false;
                }
            } else {
                if (MIX_TrackPlaying(track))
                    return true;

                if (!MIX_PlayTrack(track, 0)) {
                    error_set("Failed to play sound: %s", SDL_GetError());
                    return false;
                }
            }

            return true;
        }

        bool SdlMixerSound::pause() {
            RYO_ENSURE(valid(), false, "Invalid sound");

            if (!MIX_PauseTrack(track)) {
                error_set("Failed to pause sound: %s", SDL_GetError());
                return false;
            }

            return true;
        }

        bool SdlMixerSound::stop() {
            RYO_ENSURE(valid(), false, "Invalid sound");

            if (!MIX_StopTrack(track, 0)) {
                error_set("Failed to stop sound: %s", SDL_GetError());
                return false;
            }

            return true;
        }

        void SdlMixerSound::volume_set(float volume) {
            RYO_ENSURE_VOID(valid(), "Invalid sound");
            MIX_SetTrackGain(track, volume);
        }

        void SdlMixerSound::looping_set(bool looping) {
            RYO_ENSURE_VOID(valid(), "Invalid sound");
            MIX_SetTrackLoops(track, looping ? -1 : 0);
        }

        uint32_t SdlMixerSound::duration_miliseconds_get() const {
            RYO_ENSURE(valid(), 0, "Invalid sound");
            return static_cast<uint32_t>(MIX_AudioFramesToMS(audio->audio, MIX_GetAudioDuration(audio->audio)));
        }

        float SdlMixerSound::duration_seconds_get() const {
            RYO_ENSURE(valid(), 0.0f, "Invalid sound");
            return static_cast<float>(duration_miliseconds_get()) / 1000.0f;
        }

        uint32_t SdlMixerSound::current_miliseconds_get() const {
            RYO_ENSURE(valid(), 0, "Invalid sound");
            return static_cast<uint32_t>(MIX_AudioFramesToMS(audio->audio, MIX_GetTrackPlaybackPosition(track)));
        }

        float SdlMixerSound::current_seconds_get() const {
            RYO_ENSURE(valid(), 0.0f, "Invalid sound");
            return static_cast<float>(current_miliseconds_get()) / 1000.0f;
        }
    }
}
