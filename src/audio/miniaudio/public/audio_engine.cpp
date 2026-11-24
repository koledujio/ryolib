#include "../AudioBackendMiniaudio.h"

namespace ryo {
    namespace miniaudio_audio {
        MiniaudioBackend::MiniaudioBackend() {
            engine = new ma_engine;
            ma_result result = ma_engine_init(NULL, engine);

            if (result != MA_SUCCESS) {
                ma_engine_uninit(engine);
                delete engine;
                engine = nullptr;
                error_set("Failed to create miniaudio audio engine: %s", ma_result_description(result));
            }
        }

        MiniaudioBackend::~MiniaudioBackend() {
            if (!engine)
                return;

            ma_engine_uninit(engine);
            delete engine;
            engine = nullptr;
        }

        bool MiniaudioBackend::valid() const {
            return engine != nullptr;
        }

        bool MiniaudioBackend::master_volume_set(float volume) {
            RYO_ENSURE(valid(), false, "Invalid audio engine");
            RYO_ENSURE(volume > 0.0f, false, "Volume value is less than 0.0f");

            ma_result result = ma_engine_set_volume(engine, volume);
            if (result != MA_SUCCESS) {
                error_set("Failed to set audio engine master volume: %s", ma_result_description(result));
                return false;
            }

            return true;
        }

        SoundBackend* MiniaudioBackend::sound_create(const char* path, SoundLoadType loadType) {
            RYO_ENSURE(valid(), nullptr, "Invalid audio engine");
            return new MiniaudioSound(*this, path, loadType);
        }

        SoundBackend* MiniaudioBackend::sound_clone(const SoundBackend& source) {
            RYO_ENSURE(valid(), nullptr, "Invalid audio engine");
            return new MiniaudioSound(*this, static_cast<const MiniaudioSound&>(source));
        }
    }
}
