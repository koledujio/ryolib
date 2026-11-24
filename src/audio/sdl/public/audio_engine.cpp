#include "../AudioBackendSDL.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>

namespace ryo {
    namespace sdl_mixer_audio {
        SdlMixerBackend::SdlMixerBackend() {
            if (!MIX_Init()) {
                error_set("Failed to initialize SDL3_mixer: %s", SDL_GetError());
                return;
            }

            initialized = true;
            mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
            if (!mixer)
                error_set("Failed to create MIX_Mixer: %s", SDL_GetError());
        }

        SdlMixerBackend::~SdlMixerBackend() {
            if (mixer) {
                MIX_DestroyMixer(mixer);
                mixer = nullptr;
            }

            if (initialized) {
                MIX_Quit();
                initialized = false;
            }
        }

        bool SdlMixerBackend::valid() const {
            return mixer != nullptr;
        }

        bool SdlMixerBackend::master_volume_set(float volume) {
            RYO_ENSURE(valid(), false, "Audio Engine is not valid");
            RYO_ENSURE(volume >= 0.0f, false, "Volume is less than 0.0f");

            if (!MIX_SetMixerGain(mixer, volume)) {
                error_set("Failed to set audio engine master volume: %s", SDL_GetError());
                return false;
            }

            return true;
        }

        SoundBackend* SdlMixerBackend::sound_create(const char* path, SoundLoadType loadType) {
            RYO_ENSURE(valid(), nullptr, "Invalid audio engine");
            return new SdlMixerSound(*this, path, loadType);
        }

        SoundBackend* SdlMixerBackend::sound_clone(const SoundBackend& source) {
            RYO_ENSURE(valid(), nullptr, "Invalid audio engine");
            return new SdlMixerSound(*this, static_cast<const SdlMixerSound&>(source));
        }
    }
}
