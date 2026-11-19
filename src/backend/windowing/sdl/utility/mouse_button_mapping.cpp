#include "../private.h"

namespace ryo {
    MouseButton _mouse_get_ryo_button(unsigned char button) {
        switch (button) {
            case 1: return MOUSE_BUTTON_LEFT;
            case 2: return MOUSE_BUTTON_MIDDLE;
            case 3: return MOUSE_BUTTON_RIGHT;
            case 4: return MOUSE_BUTTON_X1;
            case 5: return MOUSE_BUTTON_X2;
            default: return MOUSE_BUTTON_NONE;
        }
    }
}
