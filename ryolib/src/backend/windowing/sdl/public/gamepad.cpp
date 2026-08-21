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
        type = _gamepad_get_ryo_type(sdlType);
        this->id = id;
    }

    Gamepad::~Gamepad() {
        if (!internal)
            return;

        if (internal->sdlGamepad)
            SDL_CloseGamepad(internal->sdlGamepad);

        delete internal;
    }

    Gamepad::Gamepad(Gamepad&& other) : internal(other.internal), id(other.id), type(other.type) {
        other.internal = nullptr;
        other.id = 0;
        other.type = PAD_TYPE_UNKNOWN;
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
        id = other.id;
        type = other.type;

        other.internal = nullptr;
        other.id = 0;
        other.type = PAD_TYPE_UNKNOWN;

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