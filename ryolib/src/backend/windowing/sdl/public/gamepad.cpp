#include "../private.h"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_sensor.h"

namespace ryo {
    ResourcePool<_InternalGamepad, MAX_GAMEPADS> _gamepad_pool;

    Gamepad gamepad_open(GamepadId id) {
        if (_gamepad_pool.active_count >= MAX_GAMEPADS) {
            error_set("Maximum number of gamepads reached");
            return {};
        }

        SDL_Gamepad* sdlGamepad = SDL_OpenGamepad(id);
        if (!sdlGamepad) {
            error_set("Failed to open SDL gamepad: %s", SDL_GetError());
            return {};
        }

        _InternalGamepad internalGamepad = {};
        internalGamepad.sdlGamepad = sdlGamepad;

        GamepadId* newId = new GamepadId(id);

        internalGamepad.id = newId;
        
        SDL_GamepadType sdlType = SDL_GetGamepadType(sdlGamepad);
        internalGamepad.type = _gamepad_get_ryo_type(sdlType);

        INDEX idx = _gamepad_pool.allocate_index();
        GENID gen = _gamepad_pool.generations[idx];

        _gamepad_pool.items[idx] = internalGamepad;

        return Gamepad::pack(idx, gen);
    }

    bool gamepad_is_valid(Gamepad gamepad) {
        INDEX idx = gamepad.index();
        GENID gen = gamepad.gen();

        if (idx >= MAX_GAMEPADS || idx == 0)
            return false;
        
        if (!_gamepad_pool.is_valid(idx, gamepad.gen()))
            return false;

        return _gamepad_pool.items[idx].sdlGamepad != nullptr && _gamepad_pool.items[idx].id != nullptr;
    }

    void gamepad_close(Gamepad gamepad) {
        RYO_ENSURE_VOID(gamepad_is_valid(gamepad), "Invalid gamepad handle");

        INDEX idx = gamepad.index();

        SDL_CloseGamepad(_gamepad_pool.items[idx].sdlGamepad);

        delete _gamepad_pool.items[idx].id;

        _gamepad_pool.free_index(idx);
    }

    GamepadId gamepad_get_id(Gamepad gamepad) {
        RYO_ENSURE(gamepad_is_valid(gamepad), 0, "Invalid gamepad handle");

        return *_gamepad_pool.items[gamepad.index()].id;
    }

    GamepadType gamepad_get_type(Gamepad gamepad) {
        RYO_ENSURE(gamepad_is_valid(gamepad), PAD_TYPE_UNKNOWN, "Invalid gamepad handle");

        return _gamepad_pool.items[gamepad.index()].type;
    }

    const char* gamepad_get_name(Gamepad gamepad) {
        RYO_ENSURE(gamepad_is_valid(gamepad), "Unknown Gamepad", "Invalid gamepad handle");

        SDL_Gamepad* sdlGamepad = _gamepad_pool.items[gamepad.index()].sdlGamepad;
        RYO_ASSERT(sdlGamepad != nullptr, "Unknown Gamepad", "SDL_Gamepad pointer is null");
        const char* name = SDL_GetGamepadName(sdlGamepad);

        return name ? name : "Unknown Gamepad";
    }

    bool gamepad_set_rumble(Gamepad gamepad, uint16_t lowFreq, uint16_t highFreq, uint32_t durationMs) {
        RYO_ENSURE(gamepad_is_valid(gamepad), false, "Invalid gamepad handle");

        SDL_Gamepad* sdlGamepad = _gamepad_pool.items[gamepad.index()].sdlGamepad;
        RYO_ASSERT(sdlGamepad != nullptr, false, "SDL_Gamepad pointer is null");

        if (!SDL_RumbleGamepad(sdlGamepad, lowFreq, highFreq, durationMs)) {
            error_set("Failed to set gamepad rumble: %s", SDL_GetError());
            return false;
        }

        return true;
    }
    
    bool gamepad_set_trigger_rumble(Gamepad gamepad, uint16_t leftTrigger, uint16_t rightTrigger, uint32_t durationMs) {
        RYO_ENSURE(gamepad_is_valid(gamepad), false, "Invalid gamepad handle");

        SDL_Gamepad* sdlGamepad = _gamepad_pool.items[gamepad.index()].sdlGamepad;
        RYO_ASSERT(sdlGamepad != nullptr, false, "SDL_Gamepad pointer is null");

        if (!SDL_RumbleGamepadTriggers(sdlGamepad, leftTrigger, rightTrigger, durationMs)) {
            error_set("Failed to set gamepad trigger rumble: %s", SDL_GetError());
            return false;
        }

        return true;
    }
}