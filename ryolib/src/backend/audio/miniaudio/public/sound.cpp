#include "../private.h"

namespace ryo {
    Sound::Sound(const AudioEngine& engine, SoundDesc soundDesc) {
        RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
        RYO_ENSURE_VOID(soundDesc.path != nullptr, "SoundDesc.path is nullptr");

        ma_sound* sound = new ma_sound;

        ma_uint32 flags = _sound_get_flags(soundDesc.loadType);
        ma_result result = ma_sound_init_from_file(
            engine.internal->engine,
            soundDesc.path,
            flags,
            NULL,
            NULL,
            sound
        );

        if (result != MA_SUCCESS) {
            error_set("Failed to load sound from path '%s': %s", soundDesc.path, ma_result_description(result));
            delete sound;
            return;
        }

        _configure_sound(sound, soundDesc.config);

        InternalSound* intern = new InternalSound;
        intern->sound = sound;

        internal = intern;
    }

    Sound::Sound(const AudioEngine& engine, const Sound& source) {
        RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
        RYO_ENSURE_VOID(source.valid(), "Invalid source sound");

        ma_sound* sourceSound = source.internal->sound;
        ma_sound* cloneSound = new ma_sound;

        ma_result result = ma_sound_init_copy(
            engine.internal->engine,
            sourceSound,
            0,
            NULL,
            cloneSound
        );

        if (result != MA_SUCCESS) {
            error_set("Failed to clone sound from path: %s", ma_result_description(result));
            delete cloneSound;
            return;
        }

        InternalSound* intern = new InternalSound;
        intern->sound = cloneSound;

        internal = intern;
    }

    Sound::Sound(Sound&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Sound& Sound::operator=(Sound&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->sound) {
                ma_sound_uninit(internal->sound);
                delete internal->sound;
            }

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    Sound::~Sound() {
        if (!internal)
            return;

        if (internal->sound) {
            ma_sound_uninit(internal->sound);
            delete internal->sound;
        }

        delete internal;
    }

    bool Sound::valid() const {
        return internal != nullptr && internal->sound != nullptr;
    }

    bool Sound::play() {
        RYO_ENSURE(valid(), false, "Invalid sound");

        ma_result seekResult = MA_SUCCESS;
        if (ma_sound_at_end(internal->sound)) {
            seekResult = ma_sound_seek_to_pcm_frame(internal->sound, 0);
        }

        ma_result result = ma_sound_start(internal->sound);
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

    bool Sound::pause() {
        RYO_ENSURE(valid(), false, "Invalid sound");

        ma_result result = ma_sound_stop(internal->sound);

        if (result != MA_SUCCESS) {
            error_set("Failed to pause sound: %s", ma_result_description(result));
            return false;
        }

        return true;
    }

    bool Sound::stop() {
        RYO_ENSURE(valid(), false, "Invalid sound");

        ma_result result = ma_sound_stop(internal->sound);
        if (result != MA_SUCCESS) {
            error_set("Failed to stop sound: %s", ma_result_description(result));
            return false;
        }

        result = ma_sound_seek_to_pcm_frame(internal->sound, 0);
        if (result != MA_SUCCESS) {
            error_set("Failed to seek sound to beginning", ma_result_description(result));
            return false;
        }

        return true;
    }

    void Sound::volume_set(float volume) {
        RYO_ENSURE_VOID(valid(), "Invalid sound");
        RYO_ENSURE_VOID(volume > 0.0f, "Volume value is less than 0.0f");

        ma_sound_set_volume(internal->sound, volume);
    }

    void Sound::looping_set(bool looping) {
        RYO_ENSURE_VOID(valid(), "Invalid sound");

        ma_sound_set_looping(internal->sound, looping ? MA_TRUE : MA_FALSE);
    }

    uint32_t Sound::duration_miliseconds_get() const {
        return static_cast<uint32_t>(duration_seconds_get() * 1000.0f);
    }

    float Sound::duration_seconds_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid sound");

        float length = 0.0f;
        ma_result result = ma_sound_get_length_in_seconds(internal->sound, &length);
        RYO_ASSERT(result == MA_SUCCESS, 0.0f, "Failed to get sound length in seconds");
        return length;
    }

    uint32_t Sound::current_miliseconds_get() const {
        return static_cast<uint32_t>(current_seconds_get() * 1000.0f);
    }

    float Sound::current_seconds_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid sound");

        float cursor = 0.0f;
        ma_result result = ma_sound_get_cursor_in_seconds(internal->sound, &cursor);
        RYO_ASSERT(result == MA_SUCCESS, 0.0f, "Failed to get sound cursor in seconds");
        return cursor;
    }
}