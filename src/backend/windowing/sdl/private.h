#pragma once

#include "../private.h"

#include "ryolib/windowing.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_metal.h>

namespace ryo {
    struct InternalApplication {
    };

    struct InternalWindow {
        SDL_Window* sdlWindow = nullptr;
        const char* title = nullptr;
        unsigned int flags = 0;
        unsigned int orientations = ORIENTATION_NONE;
        WindowID id = 0;
        SDL_MetalView metalView = nullptr;
    };

    struct InternalGamepad {
        SDL_Gamepad* sdlGamepad = nullptr;
        int id = -1;
        GamepadType type = PAD_TYPE_UNKNOWN;
    };

    extern bool sdl_initialized;
    extern uint32_t window_count;

    GamepadButton _gamepad_get_ryo_button(unsigned char event);
    GamepadType _gamepad_get_ryo_type(SDL_GamepadType type);
    GamepadAxis _gamepad_get_ryo_axis(uint8_t axis);
    KeyCode _keyboard_get_ryo_key(SDL_Scancode scancode);
    MouseButton _mouse_get_ryo_button(unsigned char button);
    const PlatformCapabilities& _platform_get_capabilities();
    bool _platform_get_forced_resolution(unsigned int& outWidth, unsigned int& outHeight);
    float _platform_get_content_scale(SDL_Window* sdlWindow);
    bool _platform_is_tablet();

    bool _sdl_initialize();
    void _sdl_quit();
}
