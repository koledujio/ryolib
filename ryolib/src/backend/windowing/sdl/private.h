#pragma once

#import "ryolib/windowing.h"
#import "ryolib/objc/RYError.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_metal.h>

typedef struct {
    SDL_Window *sdlWindow;
    RYString *title;
    unsigned int flags;
    unsigned int orientations;
    RYWindowID id;
    SDL_MetalView metalView;
} RYInternalWindow;

typedef struct {
    SDL_Gamepad *sdlGamepad;
    int id;
    RYGamepadType type;
} RYInternalGamepad;

typedef struct {
    int dummy;
} RYInternalApplication;

extern BOOL ry_sdl_initialized;

RYGamepadButton ry_gamepad_get_ryo_button(unsigned char event);
RYGamepadType ry_gamepad_get_ryo_type(SDL_GamepadType type);
RYGamepadAxis ry_gamepad_get_ryo_axis(uint8_t axis);
RYKeyCode ry_keyboard_get_ryo_key(SDL_Scancode scancode);
const RYPlatformCapabilities *ry_platform_get_capabilities(void);
BOOL ry_platform_get_forced_resolution(unsigned int *outWidth, unsigned int *outHeight);
float ry_platform_get_content_scale(SDL_Window *sdlWindow);
BOOL ry_platform_is_tablet(void);

BOOL ry_sdl_initialize(void);
void ry_sdl_quit(void);
