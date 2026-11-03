#import "../private.h"
#include <SDL3/SDL_gamepad.h>

RYGamepadAxis ry_gamepad_get_ryo_axis(uint8_t axis) {
    switch (axis) {
        case SDL_GAMEPAD_AXIS_LEFTX:
            return RYGamepadAxisLeftX;

        case SDL_GAMEPAD_AXIS_LEFTY:
            return RYGamepadAxisLeftY;

        case SDL_GAMEPAD_AXIS_RIGHTX:
            return RYGamepadAxisRightX;

        case SDL_GAMEPAD_AXIS_RIGHTY:
            return RYGamepadAxisRightY;

        case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
            return RYGamepadAxisTriggerLeft;

        case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
            return RYGamepadAxisTriggerRight;

        default:
            return RYGamepadAxisNone;
    }
}
