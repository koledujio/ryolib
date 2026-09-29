#include "../private.h"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_sensor.h"

namespace ryo {
    Gamepad::Gamepad(int id) {
        SDL_Gamepad* sdlGamepad = SDL_OpenGamepad(id);
        if (!sdlGamepad) {
            error_set("Failed to open SDL gamepad: %s", SDL_GetError());
            return;
        }

        SDL_GamepadType sdlType = SDL_GetGamepadType(sdlGamepad);

        InternalGamepad* internalGamepad = new InternalGamepad;
        internalGamepad->sdlGamepad = sdlGamepad;

        internal = internalGamepad;
        internal->type = _gamepad_get_ryo_type(sdlType);
        internal->id = id;
    }

    Gamepad::~Gamepad() {
        if (!internal)
            return;

        if (internal->sdlGamepad)
            SDL_CloseGamepad(internal->sdlGamepad);

        delete internal;
    }

    Gamepad::Gamepad(Gamepad&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Gamepad& Gamepad::operator=(Gamepad&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->sdlGamepad)
                SDL_CloseGamepad(internal->sdlGamepad);

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Gamepad::valid() const {
        return internal != nullptr && internal->sdlGamepad != nullptr;
    }

    const char* Gamepad::name_get() const {
        RYO_ENSURE(valid(), "Unknown Gamepad", "Invalid gamepad");

        const char* name = SDL_GetGamepadName(internal->sdlGamepad);

        return name ? name : "Unknown Gamepad";
    }

    int Gamepad::id_get() const {
        RYO_ENSURE(valid(), -1, "Invalid gamepad");

        return internal->id;
    }

    GamepadType Gamepad::type_get() const {
        RYO_ENSURE(valid(), PAD_TYPE_UNKNOWN, "Invalid gamepad");

        return internal->type;
    }

    bool Gamepad::rumble_set(uint16_t lowFreq, uint16_t highFreq, uint32_t durationMs) const {
        RYO_ENSURE(valid(), false, "Invalid gamepad");

        if (!SDL_RumbleGamepad(internal->sdlGamepad, lowFreq, highFreq, durationMs)) {
            error_set("Failed to set gamepad rumble: %s", SDL_GetError());
            return false;
        }

        return true;
    }
    
    bool Gamepad::rumble_set_trigger(uint16_t leftTrigger, uint16_t rightTrigger, uint32_t durationMs) const {
        RYO_ENSURE(valid(), false, "Invalid gamepad");

        if (!SDL_RumbleGamepadTriggers(internal->sdlGamepad, leftTrigger, rightTrigger, durationMs)) {
            error_set("Failed to set gamepad trigger rumble: %s", SDL_GetError());
            return false;
        }

        return true;
    }
}