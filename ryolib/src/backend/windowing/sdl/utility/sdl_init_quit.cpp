#include "../private.h"
#include <SDL3/SDL_filesystem.h>

#if defined(RYO_APPLE_IOS)
#include <unistd.h>
#endif

namespace ryo {
    bool sdl_initialized = false;

    bool _sdl_initialize() {
        SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
            return false;

#if defined(RYO_APPLE_IOS)
        const char* basePath = SDL_GetBasePath();
        if (basePath) {
            chdir(basePath);
        }
#endif

        sdl_initialized = true;
        return true;
    }

    void _sdl_quit() {
        SDL_Quit();
        sdl_initialized = false;
    }
}