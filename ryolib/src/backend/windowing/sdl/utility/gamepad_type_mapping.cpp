#include "../private.h"
#include "SDL3/SDL_gamepad.h"

namespace ryo {
    GamepadType _gamepad_get_ryo_type(SDL_GamepadType type) {
        switch (type) {
            case SDL_GAMEPAD_TYPE_COUNT:
            case SDL_GAMEPAD_TYPE_UNKNOWN:
                return PAD_TYPE_UNKNOWN;
            
            case SDL_GAMEPAD_TYPE_STANDARD:
            case SDL_GAMEPAD_TYPE_XBOX360:
                return PAD_TYPE_XBOX_360;
            
            case SDL_GAMEPAD_TYPE_XBOXONE:
                return PAD_TYPE_XBOX_ONE;
            
            case SDL_GAMEPAD_TYPE_PS3:
                return PAD_TYPE_PS3;
            
            case SDL_GAMEPAD_TYPE_PS4:
                return PAD_TYPE_PS4;
            
            case SDL_GAMEPAD_TYPE_PS5:
                return PAD_TYPE_PS5;
            
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
                return PAD_TYPE_SWITCH_PRO;
            
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
                return PAD_TYPE_SWITCH_JOYCON_LEFT;
            
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
                return PAD_TYPE_SWITCH_JOYCON_RIGHT;
            
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
                return PAD_TYPE_SWITCH_JOYCON_PAIR;
            
            case SDL_GAMEPAD_TYPE_GAMECUBE:
                return PAD_TYPE_GAMECUBE;
            
            // case SDL_GAMEPAD_TYPE_STEAM:
            //     return PAD_TYPE_STEAM;
            // to be added once sdl actually adds support
        }
    }
}