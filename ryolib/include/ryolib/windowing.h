#pragma once

#include "templates.h"

namespace ryo {
    struct InternalWindow;
    struct InternalGamepad;

    struct Event;
    
    enum GamepadType {
        PAD_TYPE_UNKNOWN = 0,
        PAD_TYPE_XBOX_360,
        PAD_TYPE_XBOX_ONE,
        PAD_TYPE_PS3,
        PAD_TYPE_PS4,
        PAD_TYPE_PS5,
        PAD_TYPE_SWITCH_PRO,
        PAD_TYPE_SWITCH_JOYCON_LEFT,
        PAD_TYPE_SWITCH_JOYCON_RIGHT,
        PAD_TYPE_SWITCH_JOYCON_PAIR,
        PAD_TYPE_GAMECUBE,
        PAD_TYPE_STEAM
    };

    struct Window {
        InternalWindow* internal = nullptr;

        Window(const char* title, unsigned int width, unsigned int height);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&);
        Window& operator=(Window&&);

        bool valid() const;

        bool event_poll(Event& event);

        void* nwh_get();
    };

    struct Gamepad {
        InternalGamepad* internal = nullptr;
        int id = 0;
        GamepadType type = PAD_TYPE_UNKNOWN;
        
        Gamepad(int id);
        ~Gamepad();

        Gamepad(const Gamepad&) = delete;
        Gamepad& operator=(const Gamepad&) = delete;

        Gamepad(Gamepad&&);
        Gamepad& operator=(Gamepad&&);

        bool valid() const;

        const char* name_get() const;

        bool rumble_set(uint16_t lowFreq, uint16_t highFreq, uint32_t durationMs) const;
        bool rumble_set_trigger(uint16_t leftTrigger, uint16_t rightTrigger, uint32_t durationMs) const;
    };

    enum EventType {
        EVENT_NONE,
        EVENT_QUIT,

        EVENT_PAD_ADDED,
        EVENT_PAD_REMOVED,
        EVENT_PAD_BUTTON_DOWN,
        EVENT_PAD_BUTTON_UP,
        EVENT_PAD_AXIS_MOTION,

        EVENT_KEY_DOWN,
        EVENT_KEY_UP
    };

    enum GamepadButton {
        PAD_BUTTON_NONE,

        PAD_BUTTON_DPAD_UP,
        PAD_BUTTON_DPAD_LEFT,
        PAD_BUTTON_DPAD_RIGHT,
        PAD_BUTTON_DPAD_DOWN,

        PAD_BUTTON_UP,
        PAD_BUTTON_LEFT,
        PAD_BUTTON_RIGHT,
        PAD_BUTTON_DOWN,

        PAD_BUTTON_START,
        PAD_BUTTON_SELECT,

        PAD_BUTTON_L1, // shoulder
        PAD_BUTTON_R1, // shoulder

        PAD_BUTTON_L3, // joystick press
        PAD_BUTTON_R3, // joystick press

        PAD_BUTTON_GUIDE
    };

    enum GamepadAxis {
        PAD_AXIS_NONE, // always 0.0
        PAD_AXIS_LEFT_X, // -1.0 (left) to 1.0 (right)
        PAD_AXIS_LEFT_Y, // -1.0 (up) to 1.0 (down)
        PAD_AXIS_RIGHT_X, // -1.0 (left) to 1.0 (right)
        PAD_AXIS_RIGHT_Y, // -1.0 (up) to 1.0 (down)
        PAD_AXIS_TRIGGER_LEFT, // 0.0 (released) to 1.0 (fully pushed)
        PAD_AXIS_TRIGGER_RIGHT // 0.0 (released) to 1.0 (fully pushed)
    };

    enum KeyCode {
        KEY_UNKNOWN = 0,

        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
        KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
        KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

        KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,

        KEY_ESCAPE, KEY_RETURN, KEY_SPACE, KEY_TAB, KEY_BACKSPACE,
        KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
        KEY_LSHIFT, KEY_RSHIFT, KEY_LCTRL, KEY_RCTRL, KEY_LALT, KEY_RALT,

        KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
        KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18, KEY_F19, KEY_F20, KEY_F21, KEY_F22, KEY_F23, KEY_F24
    };

    struct Event {
        EventType type = EVENT_NONE;

        union {
            struct {
                int id; // EVENT_PAD_ADDED, EVENT_PAD_REMOVED

                GamepadButton button; // EVENT_PAD_BUTTON_DOWN, EVENT_PAD_BUTTON_UP

                GamepadAxis axis; // EVENT_PAD_AXIS_MOTION
                float axisValue; // EVENT_PAD_AXIS_MOTION
            } pad;

            struct  {
                KeyCode code;
                bool isRepeat;
            } key;
        } data;
    };
}