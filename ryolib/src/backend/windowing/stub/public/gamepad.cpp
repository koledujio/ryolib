#include "ryolib/windowing.h"

namespace ryo {
    struct InternalGamepad {};

    Gamepad::Gamepad(int id) {}
    Gamepad::~Gamepad() {}

    Gamepad::Gamepad(Gamepad&&) {}
    Gamepad& Gamepad::operator=(Gamepad&&) {return *this;}

    bool Gamepad::valid() const {return true;}

    const char* Gamepad::name_get() const {return "Stub Controller";}

    bool Gamepad::rumble_set(uint16_t lowFreq, uint16_t highFreq, uint32_t durationMs) const {return true;}
    bool Gamepad::rumble_set_trigger(uint16_t leftTrigger, uint16_t rightTrigger, uint32_t durationMs) const {return true;}
}