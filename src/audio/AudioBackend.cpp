#include "private.h"

#include <cctype>
#include <cstdlib>

namespace ryo {
    namespace {
        struct BackendEntry {
            AudioBackendType type;
            const char* name;
            AudioEngineBackend* (*factory)();
        };

        const BackendEntry kBackends[] = {
            { AUDIO_BACKEND_MINIAUDIO, "miniaudio", audio_engine_backend_miniaudio },
            { AUDIO_BACKEND_SDL_MIXER, "sdl_mixer", audio_engine_backend_sdl_mixer },
            { AUDIO_BACKEND_STUB,      "stub",      audio_engine_backend_stub },
        };

        bool name_equals(const char* a, const char* b) {
            if (!a || !b)
                return false;

            while (*a && *b) {
                if (std::tolower(static_cast<unsigned char>(*a)) !=
                    std::tolower(static_cast<unsigned char>(*b)))
                    return false;
                ++a;
                ++b;
            }

            return *a == '\0' && *b == '\0';
        }

        const BackendEntry* find_by_type(AudioBackendType type) {
            for (const BackendEntry& entry : kBackends) {
                if (entry.type == type)
                    return &entry;
            }
            return nullptr;
        }

        const BackendEntry* find_by_name(const char* name) {
            for (const BackendEntry& entry : kBackends) {
                if (name_equals(name, entry.name))
                    return &entry;
            }
            return nullptr;
        }

        const char* backend_name_or_unknown(AudioEngineBackend* backend) {
            const char* name = backend->name();
            return name ? name : "unknown";
        }

        AudioEngineBackend* validate_builtin(AudioEngineBackend* backend) {
            if (!backend) {
                error_set("Audio backend is not compiled in");
                return nullptr;
            }

            if (!backend->is_supported()) {
                error_set("Audio backend '%s' is not supported", backend_name_or_unknown(backend));
                delete backend;
                return nullptr;
            }

            if (!backend->valid()) {
                error_set("Audio backend '%s' failed to initialize", backend_name_or_unknown(backend));
                delete backend;
                return nullptr;
            }

            return backend;
        }

        AudioEngineBackend* validate_custom(AudioEngineBackend* backend) {
            if (!backend) {
                error_set("Custom audio backend is null");
                return nullptr;
            }

            if (!backend->is_supported()) {
                error_set("Custom audio backend '%s' is not supported", backend_name_or_unknown(backend));
                return nullptr;
            }

            if (!backend->valid()) {
                error_set("Custom audio backend '%s' failed to initialize", backend_name_or_unknown(backend));
                return nullptr;
            }

            return backend;
        }
    }

#if !defined(RYOLIB_AUDIO_BACKEND_MINIAUDIO)
    AudioEngineBackend* audio_engine_backend_miniaudio() {
        return nullptr;
    }
#endif

#if !defined(RYOLIB_AUDIO_BACKEND_SDL_MIXER)
    AudioEngineBackend* audio_engine_backend_sdl_mixer() {
        return nullptr;
    }
#endif

    AudioEngineBackend* _audio_create_engine(AudioBackendType backendType, AudioEngineBackend* customBackend) {
        if (backendType == AUDIO_BACKEND_CUSTOM)
            return validate_custom(customBackend);

        if (backendType != AUDIO_BACKEND_AUTO) {
            const BackendEntry* entry = find_by_type(backendType);
            if (!entry) {
                error_set("Unknown audio backend type");
                return nullptr;
            }

            return validate_builtin(entry->factory());
        }

        if (const char* requested = std::getenv("RYO_AUDIO_BACKEND")) {
            const BackendEntry* entry = find_by_name(requested);
            if (!entry) {
                error_set("No audio backend named '%s'", requested);
                return nullptr;
            }

            return validate_builtin(entry->factory());
        }

        for (const BackendEntry& entry : kBackends) {
            if (AudioEngineBackend* backend = validate_builtin(entry.factory()))
                return backend;
        }

        error_set("No supported audio backend available");
        return nullptr;
    }
}
