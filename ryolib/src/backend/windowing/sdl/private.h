#pragma once

#include "../private.h"

#include "ryolib/windowing.h"

#include <SDL3/SDL.h>

namespace ryo {
    struct InternalWindow {
        SDL_Window* sdlWindow = nullptr;
        SDL_Event event = {}; // used in polling to not recreate for each event
    };

    struct InternalGamepad {
        SDL_Gamepad* sdlGamepad = nullptr;
    };

    extern bool sdl_initialized;
    extern uint32_t window_count;

    GamepadButton _gamepad_get_ryo_button(unsigned char event);
    GamepadType _gamepad_get_ryo_type(SDL_GamepadType type);
    GamepadAxis _gamepad_get_ryo_axis(uint8_t axis);
    KeyCode _keyboard_get_ryo_key(SDL_Scancode scancode);

    bool _sdl_initialize();
    void _sdl_quit();
}