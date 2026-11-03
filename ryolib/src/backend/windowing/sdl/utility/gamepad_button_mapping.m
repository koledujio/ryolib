#import "../private.h"

RYGamepadButton ry_gamepad_get_ryo_button(unsigned char event) {
    switch (event) {
        case SDL_GAMEPAD_BUTTON_DPAD_UP:
            return RYGamepadButtonDpadUp;

        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
            return RYGamepadButtonDpadLeft;

        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
            return RYGamepadButtonDpadRight;

        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            return RYGamepadButtonDpadDown;

        case SDL_GAMEPAD_BUTTON_NORTH:
            return RYGamepadButtonUp;

        case SDL_GAMEPAD_BUTTON_WEST:
            return RYGamepadButtonLeft;

        case SDL_GAMEPAD_BUTTON_EAST:
            return RYGamepadButtonRight;

        case SDL_GAMEPAD_BUTTON_SOUTH:
            return RYGamepadButtonDown;

        case SDL_GAMEPAD_BUTTON_START:
            return RYGamepadButtonStart;

        case SDL_GAMEPAD_BUTTON_BACK:
            return RYGamepadButtonSelect;

        case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
            return RYGamepadButtonL1;

        case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
            return RYGamepadButtonR1;

        case SDL_GAMEPAD_BUTTON_LEFT_STICK:
            return RYGamepadButtonL3;

        case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
            return RYGamepadButtonR3;

        case SDL_GAMEPAD_BUTTON_GUIDE:
            return RYGamepadButtonGuide;

        default:
            return RYGamepadButtonNone;
    }
}
