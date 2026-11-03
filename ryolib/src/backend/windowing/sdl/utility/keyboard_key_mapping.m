#import "../private.h"
#include <SDL3/SDL_scancode.h>

RYKeyCode ry_keyboard_get_ryo_key(SDL_Scancode scancode) {
    if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
        return (RYKeyCode)(RYKeyCodeA + (scancode - SDL_SCANCODE_A));
    }
    if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_0) {
        return (RYKeyCode)(RYKeyCode1 + (scancode - SDL_SCANCODE_1));
    }
    if (scancode >= SDL_SCANCODE_F1 && scancode <= SDL_SCANCODE_F12) {
        return (RYKeyCode)(RYKeyCodeF1 + (scancode - SDL_SCANCODE_F1));
    }
    if (scancode >= SDL_SCANCODE_F13 && scancode <= SDL_SCANCODE_F24) {
        return (RYKeyCode)(RYKeyCodeF13 + (scancode - SDL_SCANCODE_F13));
    }

    switch (scancode) {
        case SDL_SCANCODE_ESCAPE: return RYKeyCodeEscape;
        case SDL_SCANCODE_RETURN: return RYKeyCodeReturn;
        case SDL_SCANCODE_SPACE: return RYKeyCodeSpace;
        case SDL_SCANCODE_TAB: return RYKeyCodeTab;
        case SDL_SCANCODE_BACKSPACE: return RYKeyCodeBackspace;
        case SDL_SCANCODE_UP: return RYKeyCodeUp;
        case SDL_SCANCODE_DOWN: return RYKeyCodeDown;
        case SDL_SCANCODE_LEFT: return RYKeyCodeLeft;
        case SDL_SCANCODE_RIGHT: return RYKeyCodeRight;
        case SDL_SCANCODE_LSHIFT: return RYKeyCodeLShift;
        case SDL_SCANCODE_RSHIFT: return RYKeyCodeRShift;
        case SDL_SCANCODE_LCTRL: return RYKeyCodeLCtrl;
        case SDL_SCANCODE_RCTRL: return RYKeyCodeRCtrl;
        case SDL_SCANCODE_LALT: return RYKeyCodeLAlt;
        case SDL_SCANCODE_RALT: return RYKeyCodeRAlt;
        default: return RYKeyCodeUnknown;
    }
}
