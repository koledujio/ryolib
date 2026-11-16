#include "../private.h"

namespace ryo {
    GamepadButton _gamepad_get_ryo_button(unsigned char event) {
        switch (event) {
            case SDL_GAMEPAD_BUTTON_DPAD_UP:
                return PAD_BUTTON_DPAD_UP;
            
            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                return PAD_BUTTON_DPAD_LEFT;
            
            case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                return PAD_BUTTON_DPAD_RIGHT;
            
            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                return PAD_BUTTON_DPAD_DOWN;
            
            
            case SDL_GAMEPAD_BUTTON_NORTH:
                return PAD_BUTTON_UP;
            
            case SDL_GAMEPAD_BUTTON_WEST:
                return PAD_BUTTON_LEFT;
            
            case SDL_GAMEPAD_BUTTON_EAST:
                return PAD_BUTTON_RIGHT;
            
            case SDL_GAMEPAD_BUTTON_SOUTH:
                return PAD_BUTTON_DOWN;
            
            
            case SDL_GAMEPAD_BUTTON_START:
                return PAD_BUTTON_START;
            
            case SDL_GAMEPAD_BUTTON_BACK:
                return PAD_BUTTON_SELECT;
            
            
            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                return PAD_BUTTON_L1;
            
            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                return PAD_BUTTON_R1;

            
            case SDL_GAMEPAD_BUTTON_LEFT_STICK:
                return PAD_BUTTON_L3;
            
            case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
                return PAD_BUTTON_R3;
            

            case SDL_GAMEPAD_BUTTON_GUIDE:
                return PAD_BUTTON_GUIDE;
            
            default:
                return PAD_BUTTON_NONE;
        }
    }
}