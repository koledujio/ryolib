#include "../private.h"

namespace ryo {
    ResourcePool<ma_sound, 1024> _sound_pool;

    Sound sound_load(AudioEngine engine, SoundDesc soundDesc) {
        if (_sound_pool.active_count >= MAX_SOUNDS) {
            error_set("Failed to load sound, limit reached");
            return {};
        }

        RYO_ENSURE(audio_engine_is_valid(engine), {}, "Invalid audio engine handle");
        RYO_ENSURE(soundDesc.path != nullptr, {}, "SoundDesc.path is nullptr");

        INDEX idx = _sound_pool.allocate_index();
        GENID gen = _sound_pool.generations[idx];

        ma_uint32 flags = _sound_get_flags(soundDesc.loadType);

        ma_result result = ma_sound_init_from_file(
            &_engine_pool.items[engine.index()],
            soundDesc.path,
            flags,
            NULL,
            NULL,
            &_sound_pool.items[idx]
        );

        if (result != MA_SUCCESS) {
            error_set("Failed to load sound from path '%s': %s", soundDesc.path, ma_result_description(result));
            _sound_pool.free_index(idx);
            return {};
        }

        _configure_sound(&_sound_pool.items[idx], soundDesc.config);

        return Sound::pack(idx, gen);
    }

    Sound sound_clone(AudioEngine engine, Sound source) {
        if (_sound_pool.active_count >= MAX_SOUNDS) {
            error_set("Failed to clone sound, limit reached");
            return {};
        }

        RYO_ENSURE(audio_engine_is_valid(engine), {}, "Invalid audio engine handle");
        RYO_ENSURE(sound_is_valid(source), {}, "Invalid source sound handle");

        INDEX idx = _sound_pool.allocate_index();
        GENID gen = _sound_pool.generations[idx];

        ma_sound* sourceSound = &_sound_pool.items[source.index()];
        ma_sound* cloneSound = &_sound_pool.items[idx];

        ma_result result = ma_sound_init_copy(
            &_engine_pool.items[engine.index()],
            sourceSound,
            0,
            NULL,
            cloneSound
        );

        if (result != MA_SUCCESS) {
            error_set("Failed to clone sound from path: %s", ma_result_description(result));
            _sound_pool.free_index(idx);
            return {};
        }

        return Sound::pack(idx, gen);
    }

    void sound_destroy(Sound sound) {
        RYO_ENSURE_VOID(sound_is_valid(sound), "Invalid sound handle");

        INDEX idx = sound.index();
        ma_sound_uninit(&_sound_pool.items[idx]);
        _sound_pool.free_index(idx);
    }

    bool sound_is_valid(Sound sound) {
        INDEX idx = sound.index();
        GENID gen = sound.gen();

        return _sound_pool.is_valid(idx, gen);
    }

    bool sound_play(Sound sound) {
        RYO_ENSURE(sound_is_valid(sound), false, "Invalid sound handle");

        ma_sound* maSound = &_sound_pool.items[sound.index()];

        ma_result seekResult = MA_SUCCESS;
        if (ma_sound_at_end(maSound)) {
            seekResult = ma_sound_seek_to_pcm_frame(maSound, 0);
        }

        ma_result result = ma_sound_start(maSound);
        if (result != MA_SUCCESS && seekResult != MA_SUCCESS) {
            error_set("Failed to seek sound to beginning and start sound: %s & %s", ma_result_description(seekResult), ma_result_description(result));
            return false;
        } else if (result == MA_SUCCESS && seekResult != MA_SUCCESS) {
            error_set("Failed to seek sound to beginning: %s", ma_result_description(seekResult));
            return false;
        } else if (result != MA_SUCCESS && seekResult == MA_SUCCESS) {
            error_set("Failed to start sound: %s", ma_result_description(result));
            return false;
        }

        return true;
    }

    bool sound_pause(Sound sound) {
        RYO_ENSURE(sound_is_valid(sound), false, "Invalid sound handle");

        ma_result result = ma_sound_stop(&_sound_pool.items[sound.index()]);

        if (result != MA_SUCCESS) {
            error_set("Failed to pause sound: %s", ma_result_description(result));
            return false;
        }

        return true;
    }

    bool sound_stop(Sound sound) {
        RYO_ENSURE(sound_is_valid(sound), false, "Invalid sound handle");

        ma_sound* maSound = &_sound_pool.items[sound.index()];

        ma_result result = ma_sound_stop(maSound);
        if (result != MA_SUCCESS) {
            error_set("Failed to stop sound: %s", ma_result_description(result));
            return false;
        }

        result = ma_sound_seek_to_pcm_frame(maSound, 0);
        if (result != MA_SUCCESS) {
            error_set("Failed to seek sound to beginning", ma_result_description(result));
            return false;
        }

        return true;
    }

    void sound_set_volume(Sound sound, float volume) {
        RYO_ENSURE_VOID(sound_is_valid(sound), "Invalid sound handle");
        RYO_ENSURE_VOID(volume > 0.0f, "Volume value is less than 0.0f");

        ma_sound_set_volume(&_sound_pool.items[sound.index()], volume);
    }

    void sound_set_looping(Sound sound, bool looping) {
        RYO_ENSURE_VOID(sound_is_valid(sound), "Invalid sound handle");

        ma_sound_set_looping(&_sound_pool.items[sound.index()], looping ? MA_TRUE : MA_FALSE);
    }

    float sound_get_duration_seconds(Sound sound) {
        RYO_ENSURE(sound_is_valid(sound), 0.0f, "Invalid sound handle");

        float length = 0.0f;
        ma_result result = ma_sound_get_length_in_seconds(&_sound_pool.items[sound.index()], &length);
        RYO_ASSERT(result == MA_SUCCESS, 0.0f, "Failed to get sound length in seconds");
        return length;
    }

    float sound_get_current_seconds(Sound sound) {
        RYO_ENSURE(sound_is_valid(sound), 0.0f, "Invalid sound handle");

        float cursor = 0.0f;
        ma_result result = ma_sound_get_cursor_in_seconds(&_sound_pool.items[sound.index()], &cursor);
        RYO_ASSERT(result == MA_SUCCESS, 0.0f, "Failed to get sound cursor in seconds");
        return cursor;
    }

    uint32_t sound_get_duration_miliseconds(Sound sound) {
        return static_cast<uint32_t>(sound_get_duration_seconds(sound) * 1000.0f);
    }

    uint32_t sound_get_current_miliseconds(Sound sound) {
        return static_cast<uint32_t>(sound_get_current_seconds(sound) * 1000.0f);
    }
}