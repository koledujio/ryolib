#pragma once

#include "../private.h"

#include "ryolib/windowing.h"

#include <SDL3/SDL.h>

#include "ryolib/templates.h"

namespace ryo {
    constexpr uint32_t MAX_WINDOWS = 128;
    constexpr uint32_t MAX_GAMEPADS = 16;

    struct _InternalWindow {
        SDL_Window* sdlWindow = nullptr;
        SDL_Event event = {}; // used in polling to not recreate for each event
    };

    struct _InternalGamepad {
        SDL_Gamepad* sdlGamepad = nullptr;
        GamepadId* id = nullptr;
        GamepadType type = PAD_TYPE_UNKNOWN;
    };

    extern ResourcePool<_InternalWindow, MAX_WINDOWS> _window_pool;
    extern bool sdl_initialized;
    extern ResourcePool<_InternalGamepad, MAX_GAMEPADS> _gamepad_pool;

    GamepadButton _gamepad_get_ryo_button(unsigned char event);
    GamepadType _gamepad_get_ryo_type(SDL_GamepadType type);
    GamepadAxis _gamepad_get_ryo_axis(uint8_t axis);
    KeyCode _keyboard_get_ryo_key(SDL_Scancode scancode);

    bool _sdl_initialize();
    void _sdl_quit();
}