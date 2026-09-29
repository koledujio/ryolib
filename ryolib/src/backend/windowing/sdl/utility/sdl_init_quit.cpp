#include "../private.h"

namespace ryo {
    bool sdl_initialized = false;

    bool _sdl_initialize() {
        SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
            return false;

        sdl_initialized = true;
        return true;
    }

    void _sdl_quit() {
        SDL_Quit();
        sdl_initialized = false;
    }
}