#include "../private.h"
#include "SDL3/SDL_gamepad.h"

namespace ryo {
    GamepadAxis _gamepad_get_ryo_axis(uint8_t axis) {
        switch (axis) {
            case SDL_GAMEPAD_AXIS_LEFTX:
                return PAD_AXIS_LEFT_X;
            
            case SDL_GAMEPAD_AXIS_LEFTY:
                return PAD_AXIS_LEFT_Y;
            
            case SDL_GAMEPAD_AXIS_RIGHTX:
                return PAD_AXIS_RIGHT_X;
            
            case SDL_GAMEPAD_AXIS_RIGHTY:
                return PAD_AXIS_RIGHT_Y;
            
            case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                return PAD_AXIS_TRIGGER_LEFT;
            
            case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                return PAD_AXIS_TRIGGER_RIGHT;
            
            default:
                return PAD_AXIS_NONE;
        }
    }
}