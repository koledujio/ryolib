#include "../private.h"

#include <SDL3/SDL_error.h>

namespace ryo {
    unsigned int _num_audio_engines = 0;

    AudioEngine::AudioEngine() {
        if (_num_audio_engines == 0) {
            if (!MIX_Init()) {
                error_set("Failed to initialize SDL3_mixer: %s", SDL_GetError());
                return;
            }
        }

        MIX_Mixer* mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (!mixer) {
            error_set("Failed to create MIX_Mixer: %s", SDL_GetError());

            if (_num_audio_engines == 0)
                MIX_Quit();

            return;
        }

        InternalAudioEngine* intern = new InternalAudioEngine;
        intern->mixer = mixer;

        internal = intern;

        _num_audio_engines++;
    }

    AudioEngine::~AudioEngine() {
        if (!internal)
            return;

        if (internal->mixer)
            MIX_DestroyMixer(internal->mixer);

        delete internal;

        if (_num_audio_engines == 1)
            MIX_Quit();

        _num_audio_engines--;
    }

    AudioEngine::AudioEngine(AudioEngine&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    AudioEngine& AudioEngine::operator=(AudioEngine&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->mixer)
                MIX_DestroyMixer(internal->mixer);

            delete internal;

            if (_num_audio_engines == 1)
                MIX_Quit();

            _num_audio_engines--;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool AudioEngine::valid() const {
        return internal != nullptr && internal->mixer != nullptr;
    }

    bool AudioEngine::master_volume_set(float volume) {
        RYO_ENSURE(valid(), false, "Audio Engine is not valid");
        RYO_ENSURE(volume >= 0.0f, false, "Volume is less than 0.0f");

        if (!MIX_SetMixerGain(internal->mixer, volume)) {
            error_set("Failed to set audio engine master volume: %s", SDL_GetError());
            return false;
        }

        return true;
    }
}