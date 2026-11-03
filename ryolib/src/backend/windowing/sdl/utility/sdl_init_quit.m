#import "../private.h"
#include <SDL3/SDL_filesystem.h>

#if defined(RYO_APPLE_IOS)
#include <unistd.h>
#endif

BOOL ry_sdl_initialized = NO;

BOOL ry_sdl_initialize(void) {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        return NO;

#if defined(RYO_APPLE_IOS)
    const char *basePath = SDL_GetBasePath();
    if (basePath) {
        chdir(basePath);
    }
#endif

    ry_sdl_initialized = YES;
    return YES;
}

void ry_sdl_quit(void) {
    SDL_Quit();
    ry_sdl_initialized = NO;
}
