#include "ryolib/windowing.h"

namespace ryo {
    struct InternalGamepad {};

    Gamepad::Gamepad(const Application& app, int id) {}
    Gamepad::~Gamepad() {}

    Gamepad::Gamepad(Gamepad&&) {}
    Gamepad& Gamepad::operator=(Gamepad&&) {return *this;}

    bool Gamepad::valid() const {return false;}

    const char* Gamepad::name_get() const {return nullptr;}

    int Gamepad::id_get() const {return -1;}

    GamepadType Gamepad::type_get() const {return PAD_TYPE_UNKNOWN;}

    bool Gamepad::rumble_set(uint16_t lowFreq, uint16_t highFreq, uint32_t durationMs) const {return false;}
    bool Gamepad::rumble_set_trigger(uint16_t leftTrigger, uint16_t rightTrigger, uint32_t durationMs) const {return false;}
}