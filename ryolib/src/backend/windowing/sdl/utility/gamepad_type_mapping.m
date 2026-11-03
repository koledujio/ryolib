#import "../private.h"
#include <SDL3/SDL_gamepad.h>

RYGamepadType ry_gamepad_get_ryo_type(SDL_GamepadType type) {
    switch (type) {
        case SDL_GAMEPAD_TYPE_COUNT:
        case SDL_GAMEPAD_TYPE_UNKNOWN:
            return RYGamepadTypeUnknown;

        case SDL_GAMEPAD_TYPE_STANDARD:
        case SDL_GAMEPAD_TYPE_XBOX360:
            return RYGamepadTypeXbox360;

        case SDL_GAMEPAD_TYPE_XBOXONE:
            return RYGamepadTypeXboxOne;

        case SDL_GAMEPAD_TYPE_PS3:
            return RYGamepadTypePS3;

        case SDL_GAMEPAD_TYPE_PS4:
            return RYGamepadTypePS4;

        case SDL_GAMEPAD_TYPE_PS5:
            return RYGamepadTypePS5;

        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
            return RYGamepadTypeSwitchPro;

        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
            return RYGamepadTypeSwitchJoyConLeft;

        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
            return RYGamepadTypeSwitchJoyConRight;

        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
            return RYGamepadTypeSwitchJoyConPair;

        case SDL_GAMEPAD_TYPE_GAMECUBE:
            return RYGamepadTypeGameCube;

        default:
            return RYGamepadTypeUnknown;
    }
}
