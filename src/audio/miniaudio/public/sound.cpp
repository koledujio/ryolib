#include "../AudioBackendMiniaudio.h"

namespace ryo {
    namespace miniaudio_audio {
        MiniaudioSound::MiniaudioSound(MiniaudioBackend& engine, const char* path, SoundLoadType loadType) {
            RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
            RYO_ENSURE_VOID(path != nullptr, "Sound path is nullptr");

            sound = new ma_sound;

            ma_uint32 flags = _sound_get_flags(loadType);
            ma_result result = ma_sound_init_from_file(
                engine.engine,
                path,
                flags,
                NULL,
                NULL,
                sound
            );

            if (result != MA_SUCCESS) {
                error_set("Failed to load sound from path '%s': %s", path, ma_result_description(result));
                delete sound;
                sound = nullptr;
                return;
            }
        }

        MiniaudioSound::MiniaudioSound(MiniaudioBackend& engine, const MiniaudioSound& source) {
            RYO_ENSURE_VOID(engine.valid(), "Invalid audio engine");
            RYO_ENSURE_VOID(source.valid(), "Invalid source sound");

            sound = new ma_sound;

            ma_result result = ma_sound_init_copy(
                engine.engine,
                source.sound,
                0,
                NULL,
                sound
            );

            if (result != MA_SUCCESS) {
                error_set("Failed to clone sound from path: %s", ma_result_description(result));
                delete sound;
                sound = nullptr;
                return;
            }
        }

        MiniaudioSound::~MiniaudioSound() {
            if (!sound)
                return;

            ma_sound_uninit(sound);
            delete sound;
            sound = nullptr;
        }

        bool MiniaudioSound::valid() const {
            return sound != nullptr;
        }

        bool MiniaudioSound::play() {
            RYO_ENSURE(valid(), false, "Invalid sound");

            ma_result seekResult = MA_SUCCESS;
            if (ma_sound_at_end(sound)) {
                seekResult = ma_sound_seek_to_pcm_frame(sound, 0);
            }

            ma_result result = ma_sound_start(sound);
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

        bool MiniaudioSound::pause() {
            RYO_ENSURE(valid(), false, "Invalid sound");

            ma_result result = ma_sound_stop(sound);
            if (result != MA_SUCCESS) {
                error_set("Failed to pause sound: %s", ma_result_description(result));
                return false;
            }

            return true;
        }

        bool MiniaudioSound::stop() {
            RYO_ENSURE(valid(), false, "Invalid sound");

            ma_result result = ma_sound_stop(sound);
            if (result != MA_SUCCESS) {
                error_set("Failed to stop sound: %s", ma_result_description(result));
                return false;
            }

            result = ma_sound_seek_to_pcm_frame(sound, 0);
            if (result != MA_SUCCESS) {
                error_set("Failed to seek sound to beginning", ma_result_description(result));
                return false;
            }

            return true;
        }

        void MiniaudioSound::volume_set(float volume) {
            RYO_ENSURE_VOID(valid(), "Invalid sound");
            RYO_ENSURE_VOID(volume > 0.0f, "Volume value is less than 0.0f");
            ma_sound_set_volume(sound, volume);
        }

        void MiniaudioSound::looping_set(bool looping) {
            RYO_ENSURE_VOID(valid(), "Invalid sound");
            ma_sound_set_looping(sound, looping ? MA_TRUE : MA_FALSE);
        }

        uint32_t MiniaudioSound::duration_miliseconds_get() const {
            return static_cast<uint32_t>(duration_seconds_get() * 1000.0f);
        }

        float MiniaudioSound::duration_seconds_get() const {
            RYO_ENSURE(valid(), 0.0f, "Invalid sound");

            float length = 0.0f;
            ma_result result = ma_sound_get_length_in_seconds(sound, &length);
            RYO_ASSERT(result == MA_SUCCESS, 0.0f, "Failed to get sound length in seconds");
            return length;
        }

        uint32_t MiniaudioSound::current_miliseconds_get() const {
            return static_cast<uint32_t>(current_seconds_get() * 1000.0f);
        }

        float MiniaudioSound::current_seconds_get() const {
            RYO_ENSURE(valid(), 0.0f, "Invalid sound");

            float cursor = 0.0f;
            ma_result result = ma_sound_get_cursor_in_seconds(sound, &cursor);
            RYO_ASSERT(result == MA_SUCCESS, 0.0f, "Failed to get sound cursor in seconds");
            return cursor;
        }
    }
}
