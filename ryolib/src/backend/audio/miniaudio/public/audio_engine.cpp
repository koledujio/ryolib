#include "../private.h"

namespace ryo {
    ResourcePool<ma_engine, MAX_ENGINES> _engine_pool;

    AudioEngine audio_engine_create() {
        if (_engine_pool.active_count >= MAX_ENGINES) {
            error_set("Maximum number of audio engines reached");
            return {};
        }

        INDEX idx = _engine_pool.allocate_index();
        GENID gen = _engine_pool.generations[idx];

        ma_result result = ma_engine_init(NULL, &_engine_pool.items[idx]);

        if (result != MA_SUCCESS) {
            ma_engine_uninit(&_engine_pool.items[idx]);
            error_set("Failed to create miniaudio audio engine: %s", ma_result_description(result));
            _engine_pool.free_index(idx);
            return {};
        }

        return AudioEngine::pack(idx, gen);
    }

    void audio_engine_destroy(AudioEngine engine) {
        RYO_ENSURE_VOID(audio_engine_is_valid(engine), "Invalid audio engine handle");

        ma_engine_uninit(&_engine_pool.items[engine.index()]);
        _engine_pool.free_index(engine.index());
    }

    bool audio_engine_is_valid(AudioEngine engine) {
        INDEX idx = engine.index();
        GENID gen = engine.gen();

        if (idx >= MAX_ENGINES) 
            return false;

        return _engine_pool.is_valid(idx, gen);
    }

    bool audio_engine_set_master_volume(AudioEngine engine, float volume) {
        RYO_ENSURE(audio_engine_is_valid(engine), false, "Invalid audio engine handle");
        RYO_ENSURE(volume > 0.0f, false, "Volume value is less than 0.0f");

        ma_result result = ma_engine_set_volume(&_engine_pool.items[engine.index()], volume);

        if (result != MA_SUCCESS) {
            error_set("Failed to set audio engine master volume: %s", ma_result_description(result));
            return false;
        }

        return true;
    }
}