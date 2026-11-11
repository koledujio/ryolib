#include "../private.h"

namespace ryo {
    AudioEngine::AudioEngine() {
        ma_engine* engine = new ma_engine;
        ma_result result = ma_engine_init(NULL, engine);

        if (result != MA_SUCCESS) {
            ma_engine_uninit(engine);
            delete engine;
            error_set("Failed to create miniaudio audio engine: %s", ma_result_description(result));
            return;
        }

        InternalAudioEngine* intern = new InternalAudioEngine;
        intern->engine = engine;

        internal = intern;
    }

    AudioEngine::~AudioEngine() {
        if (internal)
            return;

        if (internal->engine) {
            ma_engine_uninit(internal->engine);
            delete internal->engine;
        }

        delete internal;
    }

    AudioEngine::AudioEngine(AudioEngine&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    AudioEngine& AudioEngine::operator=(AudioEngine&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->engine) {
                ma_engine_uninit(internal->engine);
                delete internal->engine;
            }

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool AudioEngine::valid() const {
        return internal != nullptr && internal->engine != nullptr;
    }

    bool AudioEngine::master_volume_set(float volume) {
        RYO_ENSURE(valid(), false, "Invalid audio engine");
        RYO_ENSURE(volume > 0.0f, false, "Volume value is less than 0.0f");

        ma_result result = ma_engine_set_volume(internal->engine, volume);

        if (result != MA_SUCCESS) {
            error_set("Failed to set audio engine master volume: %s", ma_result_description(result));
            return false;
        }

        return true;
    }
}