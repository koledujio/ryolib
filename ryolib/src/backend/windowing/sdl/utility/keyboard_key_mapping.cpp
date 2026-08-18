#include "../private.h"
#include "SDL3/SDL_scancode.h"

namespace ryo {
    KeyCode _keyboard_get_ryo_key(SDL_Scancode scancode) {
        if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
            return static_cast<KeyCode>(KEY_A + (scancode - SDL_SCANCODE_A));
        }
        if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_0) {
            return static_cast<KeyCode>(KEY_1 + (scancode - SDL_SCANCODE_1));
        }
        if (scancode >= SDL_SCANCODE_F1 && scancode <= SDL_SCANCODE_F12) {
            return static_cast<KeyCode>(KEY_F1 + (scancode - SDL_SCANCODE_F1));
        }
        if (scancode >= SDL_SCANCODE_F13 && scancode <= SDL_SCANCODE_F24) {
            return static_cast<KeyCode>(KEY_F13 + (scancode - SDL_SCANCODE_F13));
        }

        switch (scancode) {
            case SDL_SCANCODE_ESCAPE: return KEY_ESCAPE;
            case SDL_SCANCODE_RETURN: return KEY_RETURN;
            case SDL_SCANCODE_SPACE: return KEY_SPACE;
            case SDL_SCANCODE_TAB: return KEY_TAB;
            case SDL_SCANCODE_BACKSPACE: return KEY_BACKSPACE;
            case SDL_SCANCODE_UP: return KEY_UP;
            case SDL_SCANCODE_DOWN: return KEY_DOWN;
            case SDL_SCANCODE_LEFT: return KEY_LEFT;
            case SDL_SCANCODE_RIGHT: return KEY_RIGHT;
            case SDL_SCANCODE_LSHIFT: return KEY_LSHIFT;
            case SDL_SCANCODE_RSHIFT: return KEY_RSHIFT;
            case SDL_SCANCODE_LCTRL: return KEY_LCTRL;
            case SDL_SCANCODE_RCTRL: return KEY_RCTRL;
            case SDL_SCANCODE_LALT: return KEY_LALT;
            case SDL_SCANCODE_RALT: return KEY_RALT;
            default: return KEY_UNKNOWN;
        }
    }
}