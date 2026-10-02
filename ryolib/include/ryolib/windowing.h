#pragma once

#include "templates.h"

namespace ryo {
    /**
     * Additional Types.
     */

    using WindowID = uint32_t;

    /**
     * Forward declarations.
     */

    struct InternalApplication;
    struct InternalWindow;
    struct InternalGamepad;

    /**
     * Platform and hardware capabilities.
     */

    /**
     * @brief Platform type.
     *
     * @see Window
     * @see PlatformCapability
     * @see Window::capabilities_get()
     */
    enum PlatformType {
        PLATFORM_UNKNOWN, ///< Unknown platform.

        PLATFORM_MACOS,   ///< macOS.
        PLATFORM_WINDOWS, ///< Windows.
        PLATFORM_VITA,    ///< Playstation Vita.
        PLATFORM_IOS      ///< iOS/iPadOS.
    };

    /**
     * @brief Device type.
     *
     * @see Window
     * @see PlatformCapability
     * @see Window::capabilities_get()
     */
    enum DeviceType {
        DEVICE_UNKNOWN, ///< Unknown device type.

        DEVICE_DESKTOP, ///< Desktop computer.
                        ///< Currently includes: macOS, Windows.

        DEVICE_MOBILE,  ///< Mobile phone/tablet.
                        ///< Currently includes: iOS/iPadOS.

        DEVICE_CONSOLE  ///< Console.
                        ///< Currently includes: Playstation Vita.
    };

    /**
     * @brief Window size managment type.
     *
     * @see Window
     * @see PlatformCapability
     * @see Window::capabilitites_get()
     */
    enum SizeManagementType {
        SIZE_FREE_FIXED,       ///< Size set manually, not resizable.
                               ///< Includes: none.

        SIZE_FREE_RESIZABLE,   ///< Size set manually, resizable.
                               ///< Includes: macOS, Windows.

        SIZE_MANAGED_FIXED,    ///< Size dictated by OS, not resizable.
                               ///< Includes: Playstation Vita.

        SIZE_MANAGED_RESIZABLE ///< Size dictated by OS, resizable.
                               ///< Includes: iOS/iPadOS.
    };

    /**
     * @brief Platform capabilities.
     *
     * The platform's capabilities and device information.
     * This data isn't changed since the window has been created and is the same across every window,
     * it's recommended to cache it after creating the first window and reuse it per instance of app/program.
     */
    struct PlatformCapabilities {
        PlatformType platformType;             ///< Platform / operating system.
        DeviceType deviceType;                 ///< Device form factor.
        SizeManagementType sizeManagementType; ///< Sizing and resizing behavior.

        /**
         * @brief Tablet form factor.
         *
         * True for tablet devices, False for everything else.
         * Currently True for: iPadOS.
         * False for: macOS, Windows, Playstation Vita, iOS/iPhone.
         *
         * @note Tablets support multitasking (Split View, Slide Over, Stage Manager),
         * which is why they are allowed to support every display orientation, refer to
         * @ref DisplayOrientations.
         */
        bool isTablet;

        /**
         * @brief Fixed default orientation.
         *
         * True, if the platform supports only one default orientation.
         * False, if the platform supports more than one default orientation.
         *
         * True for: macOS, Windows, Playstation Vita.
         * False for: iOS/iPadOS.
         */
        bool fixedDefaultOrientation;

        /**
         * @brief Support for multiple windows.
         *
         * Specifies weather the platform supports multiple windows.
         * 
         * True for: macOS, Windows.
         * False for: Playstation Vita, iOS/iPadOS.
         */
        bool supportsMultipleWindows;

        /**
         * @brief Specifies if the platform supports gamepad controllers.
         *
         * True for: macOS, Windows, Playstation Vita, iOS/iPadOS.
         * False for: none.
         */
        bool controllerSupport;

        /**
         * @brief Specifies if the platform supports keyboards.
         *
         * True for: macOS, Windows, Playstation Vita, iOS/iPadOS.
         * False for: none.
         */
        bool keyboardSupport;

        /**
         * @brief Specifies if the platform supports mouse input.
         *
         * True for: macOS, Windows, Playstation Vita, iOS/iPadOS.
         * False for: none.
         */
        bool mouseSupport;

        /**
         * @brief Specifies if the platform supports touch input.
         *
         * True for: Windows, Playstation Vita, iOS/iPadOS.
         * False for: macOS.
         */
        bool touchSupport;

        /**
         * @brief Specifies if the platform supports custom window titles.
         *
         * True for: macOS, Windows.
         * False for: Playstation Vita, iOS/iPadOS.
         */
        bool windowTitlesSupported;

        /**
         * @brief Specifies if the platform supports native window handles.
         *
         * True for macOS, Windows, iOS/iPadOS.
         * False for: Playstation Vita.
         */
        bool nativeWindowHandleSupported;

        /**
         * @brief Specifies if the platform supports application invoked application quitting.
         *
         * True for: macOS, Windows, Playstation Vita.
         * False for: iOS/iPadOS
         */
        bool quitSupported;
    };

    /**
     * Window and display configuration.
     */

    /**
     * @brief Window behavior flags bitmask.
     *
     * @note None of these apply for console and mobile, the flags are safely ignored,
     * with the exception of @ref WindowFlags::WINDOW_FLAG_STRICT_ORIENTATIONS, which
     * applies to iPadOS.
     *
     * @see Window()
     * @see Window::flags_get()
     * @see Window::flags_set()
     */
    enum WindowFlags : unsigned int {
        WINDOW_FLAG_NONE       = 0,      ///< Default window with no special attributes.
        WINDOW_FLAG_RESIZABLE  = 1 << 0, ///< Allows user resizing and using minimum/maximum window dimensions.
        WINDOW_FLAG_FULLSCREEN = 1 << 1, ///< Spans the entire screen or native display.
        WINDOW_FLAG_BORDERLESS = 1 << 2, ///< Removes window borders and decorations.

        /**
         * @brief Honors the requested display orientations on iPadOS.
         *
         * Applications are required to support every display orientation, so that
         * they can participate in multitasking, which is the default. This flag
         * disables that and honors the requested @ref DisplayOrientations on iPadOS,
         * at the cost of disabling multitasking.
         */
        WINDOW_FLAG_STRICT_ORIENTATIONS = 1 << 3
    };

    /**
     * @brief Display orientations bitmask.
     *
     * @note In order to support multitasking (Split View, Slide Over, Stage Manager) on iPadOS,
     * the application needs to support every orientation. By default for iPadOS, the allowed orientations
     * are ignored and are automatically set to @ref DisplayOrientations::ORIENTATION_ALL unless the
     * @ref WindowFlags::WINDOW_FLAG_STRICT_ORIENTATIONS flag is set.
     *
     * @see Window()
     * @see Window::orientations_get()
     * @see Window::orientations_set()
     * @see Event
     */
    enum DisplayOrientations : unsigned int {
        ORIENTATION_NONE              = 0,      ///< Default orientation for devices without orientation support.
        ORIENTATION_LANDSCAPE         = 1 << 0, ///< Landscape display orientation
        ORIENTATION_PORTRAIT          = 1 << 1, ///< Portrait display orientation
        ORIENTATION_LANDSCAPE_FLIPPED = 1 << 2, ///< Flipped landscape display orientation
        ORIENTATION_PORTRAIT_FLIPPED  = 1 << 3, ///< Flipped portrait display orientation
        ORIENTATION_ALL               = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) ///< Every display orientation.
    };

    /**
     * @brief Safe area insets.
     *
     * The window's safe area insets in logical pixels.
     * Changes based on window dimensions, display orientation and folding state.
     *
     * TODO: add safe area insets changed event
     *
     * @see Window
     * @see Window::safe_area_insets_get()
     */
    struct SafeAreaInsets {
        float top = 0.0f;
        float bottom = 0.0f;
        float left = 0.0f;
        float right = 0.0f;
    };

    /**
     * Input definitions.
     */

    /**
     * @brief Keyboard key codes.
     *
     * @see Event
     * @see EventType
     */
    enum KeyCode {
        KEY_UNKNOWN = 0, ///< No key / unrecognized key.

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

    /**
     * @brief Gamepad types.
     *
     * @see Gamepad
     * @see Event
     */
    enum GamepadType {
        PAD_TYPE_UNKNOWN = 0,         ///< Unrecognized controller.
        PAD_TYPE_XBOX_360,            ///< Xbox 360 controller.
        PAD_TYPE_XBOX_ONE,            ///< Xbox One / Xbox Series S/X controller.
        PAD_TYPE_PS3,                 ///< Playstation 3 controller.
        PAD_TYPE_PS4,                 ///< Playstation 4 controller.
        PAD_TYPE_PS5,                 ///< Playstation 5 controller.
        PAD_TYPE_SWITCH_PRO,          ///< Nintendo Switch Pro Controller.
        PAD_TYPE_SWITCH_JOYCON_LEFT,  ///< Nintendo Switch Left Joy-Con.
        PAD_TYPE_SWITCH_JOYCON_RIGHT, ///< Nintendo Switch Right Joy-Con.
        PAD_TYPE_SWITCH_JOYCON_PAIR,  ///< Nintendo Switch Joy-Con Pair.
        PAD_TYPE_GAMECUBE,            ///< Nintendo Gamecube Controller.
        PAD_TYPE_STEAM                ///< Steam Controller.
    };

    /**
     * @brief Gamepad controller button types.
     *
     * @see Gamepad
     * @see Event
     * @see EventType
     * @see GamepadAxis
     */
    enum GamepadButton {
        PAD_BUTTON_NONE,       ///< No button pressed / unrecognized button type.

        PAD_BUTTON_DPAD_UP,    ///< Directional pad up button.
        PAD_BUTTON_DPAD_DOWN,  ///< Directional pad down button.
        PAD_BUTTON_DPAD_LEFT,  ///< Directional pad left button.
        PAD_BUTTON_DPAD_RIGHT, ///< Directional pad right button.

        PAD_BUTTON_UP,         ///< Face up button.
        PAD_BUTTON_LEFT,       ///< Face left button.
        PAD_BUTTON_RIGHT,      ///< Face right button.
        PAD_BUTTON_DOWN,       ///< Face down button.

        PAD_BUTTON_START,      ///< Start button.
        PAD_BUTTON_SELECT,     ///< Select button.

        PAD_BUTTON_L1,         ///< Left shoulder button.
        PAD_BUTTON_R1,         ///< Right shoulder button.

        PAD_BUTTON_L3,         ///< Left analog stick press.
        PAD_BUTTON_R3,         ///< Right analog stick press.

        PAD_BUTTON_GUIDE       ///< Guide button.
    };

    /**
     * @brief Gamepad axis types.
     *
     * TODO: figure out how to handle analog-digital triggers and other analog/digital stuff. Currently both digital and analog triggers use GamepadAxis
     *
     * @see Gamepad
     * @see Event
     * @see EventType
     * @see GamepadButton
     */
    enum GamepadAxis {
        PAD_AXIS_NONE,        ///< No axis, unrecognized axis.
                              ///< Always 0.0f.

        PAD_AXIS_LEFT_X,      ///< Left analog stick horizontal axis.
                              ///< Ranges from -1.0f(left) to 1.0f(right).
        PAD_AXIS_LEFT_Y,      ///< Left analog stick vertical axis.
                              ///< Ranges from -1.0f(up) to 1.0f(down).

        PAD_AXIS_RIGHT_X,     ///< Right analog stick horizontal axis.
                              ///< Ranges from -1.0f(left) to 1.0f(right).
        PAD_AXIS_RIGHT_Y,     ///< Right analog stick vertical axis.
                              ///< Ranges from -1.0f(up) to 1.0f(down).

        PAD_AXIS_TRIGGER_LEFT, ///< Left analog trigger push-in axis.
                               ///< Ranges from 0.0f(released) to 1.0f(fully pushed in).
        PAD_AXIS_TRIGGER_RIGHT ///< Right analog trigger push-in axis.
                               ///< Ranges from 0.0f(released) to 1.0f(fully pushed in).
    };

    /**
     * Event system.
     */

    /**
     * @brief Type of @ref Event
     *
     * TODO: add safe area insets changed event
     *
     * @see Event
     * @see Window
     * @see Window::event_poll()
     */
    enum EventType {
        /**
         * @brief No event occurred / empty event.
         *
         * Indicates that the event contains no actionable data and can be safely ignored and discarded.
         */
        EVENT_NONE,

        /**
         * @brief Application termination request.
         *
         * Triggered when the OS or user requests the window or application to close.
         * (e.g., clicking the close button in the window decorations, Alt+F4, Command+Q, Command+W).
         *
         * TODO: figure out how to handle mac-style windowless apps / differenciate window close from application close.
         */
        EVENT_QUIT,

        /**
         * @brief Window resized.
         *
         * Triggered when the OS or user requests the window to be resized.
         * Populates the window id in @ref Event::windowId and width and height in logical
         * pixels in @ref Event::data::window::width and @ref Event::data::window::height;
         *
         * @note This can only be triggered if the window has the @ref WindowFlags::WINDOW_FLAG_RESIZEABLE
         * flag enabled and the @ref PlatformCapapabilities::sizeManagmentType is either
         * @ref SizeManagmentType::SIZE_FREE_RESIZABLE or @ref SizeManagmentType::SIZE_MANAGED_RESIZABLE.
         *
         * @see Window
         * @see WindowFlags
         * @see PlatformCapabilities
         * @see SizeManagmentType
         */
        EVENT_WINDOW_RESIZED,

        /**
         * @brief Window content (HiDPI) scale changed.
         *
         * Triggered when the window's content scale has changed, which is the number of
         * physical pixels the window covers per logical pixel. Refer to
         * @ref Window::hidpi_scale_get() to retrieve the new value.
         * Populates the window id in @ref Event::windowId.
         *
         * @note The content scale depends on the display the window is on, so this can be
         * triggered by moving the window between displays with different pixel densities or
         * by the display's scaling changing.
         * @note Platforms with a fixed display density (mobile and console) don't change
         * their content scale, so this event type isn't triggered there. Their render
         * resolution follows the window size instead, refer to
         * @ref EventType::EVENT_WINDOW_RESIZED.
         * @note Content rasterized at a logical size (text, for example) has to be
         * regenerated after this event to stay at the new density.
         *
         * @see Window
         * @see Window::hidpi_scale_get()
         * @see EventType::EVENT_WINDOW_RESIZED
         */
        EVENT_WINDOW_HIDPI_SCALE_CHANGED,

        /**
         * @brief New gamepad controller available.
         *
         * Triggered when a gamepad becomes available for the window to connect.
         * The gamepad only starts emitting @ref EventType::EVENT_PAD_BUTTON_DOWN,
         * @ref EventType::EVENT_PAD_BUTTON_UP and @ref EventType::EVENT_PAD_AXIS_MOTION
         * if the window explicitly connects to the gamepad controller using it's gamepad id by
         * creating a @ref Gamepad object.
         * Populates the gamepad id in @ref Event::data::pad::id.
         *
         * @note If one or more gamepad controllers are currently connected at the time of window creation,
         * this event type will be added to the event queue in order to allow the window to connect with already connected
         * gamepad controllers to the device.
         *
         * @see Gamepad 
         */
        EVENT_PAD_ADDED,

        /**
         * @brief Gamepad controller unavailable.
         *
         * Triggered when a gamepad becomes unavailable for the window to use.
         * If the gamepad that has become unavailable was connected to the window by
         * creating a @ref Gamepad object, the connected @ref Gamepad object must be destroyed.
         * Populates the gamepad id in @ref Event::data::pad::id.
         *
         * @note This event type can be triggered even if the gamepad hasn't been connected by the window,
         * in which case this event can be safely ignored and discarded.
         *
         * @see Gamepad
         */
        EVENT_PAD_REMOVED,

        /**
         * @brief Gamepad controller button pressed down.
         *
         * Triggered when a gamepad's button has been pressed down.
         * Populates the gamepad id in @ref Event::data::pad::id and button type in @ref Event::data::pad::button.
         *
         * @note This event type is only triggered for gamepads that have been connected to by
         * creating a @ref Gamepad object.
         *
         * @see Gamepad
         * @see GamepadButton
         */
        EVENT_PAD_BUTTON_DOWN,

        /**
         * @brief Gamepad controller button released.
         *
         * Triggered when a gamepad's button has been released.
         * Populates the gamepad id in @ref Event::data::pad::id and button type in @ref Event::data::pad::button.
         *
         * @note This event type is only triggered for gamepads that have been connected to by
         * creating a @ref Gamepad object.
         *
         * @see Gamepad
         * @see GamepadButton
         */
        EVENT_PAD_BUTTON_UP,

        /**
         * @brief Gamepad controller's axis has changed.
         *
         * Triggered when a gamepad's axis has changed.
         * A gamepad controller's axis includes controls such as analog sticks and triggers.
         * The value which @ref Event::data::pad::axisValue represents is based on the @ref Event::data::pad::axis,
         * and is documented in @ref GamepadAxis.
         * Populates the gamepad id in @ref Event::data::pad::id, axis type in @ref Event::data::pad::axis,
         * and axis value in @ref Event::data::pad::axisValue.
         *
         * TODO: figure out how to handle ps2/xbox controller's analog buttons.
         *
         * @note This event type is only triggered for gamepads that have been connected to by
         * creating a @ref Gamepad object.
         *
         * @see Gamepad
         * @see GamepadAxis
         */
        EVENT_PAD_AXIS_MOTION,

        /**
         * @brief Keyboard key pressed.
         *
         * Triggered when a keyboard key has been pressed.
         * If the key wasn't physically just pressed and instead the OS repeats the input, the repeat flag is
         * set to true.
         * Populates the window id in @ref Event::windowId, key code in @ref Event::data::key::code
         * and the repeat flag in @ref Event::data::key::isRepeat.
         *
         * @see KeyCode.
         */
        EVENT_KEY_DOWN,

        /**
         * @brief Keyboard key released.
         *
         * Triggered when a keyboard key has been released.
         * Populates the window id in @ref Event::windowId and the key code in @ref Event::data::key::code.
         *
         * @see KeyCode.
         */
        EVENT_KEY_UP,

        /**
         * @brief Touch finger placed down on screen.
         *
         * Triggered when a new touch finger has been placed down on the screen.
         * This event type uses normalized coordinates from -1.0f to 1.0f.
         * If the normalized coordinate represents the x value, -1.0f is the left edge of the screen, and 1.0f is the
         * right edge, else if the normalized coordinate represents the y value, -1.0f is the top edge, and 1.0f is the bottom edge.
         * Populates the window id in @ref Event::windowId, finger id in @ref Event::data::touch::fingerId and normalized coordinates
         * in @ref Event::data::touch::x and @ref Event::data::touch::y.
         */
        EVENT_TOUCH_BEGIN,

        /**
         * @brief Touch finger moved across screen.
         *
         * Triggered when a touch finger has moved across the screen.
         * This event type uses normalized coordinates from -1.0f to 1.0f.
         * If the normalized coordinate represents the x value, -1.0f is the left edge of the screen, and 1.0f is the
         * right edge, else if the normalized coordinate represents the y value, -1.0f is the top edge, and 1.0f is the bottom edge.
         * The @ref Event::data::touch::deltaX and @ref Event::data::touch::deltaY values represent the amount of how much the normalized
         * position of the touch finger has changed since the last @ref EVENT_TOUCH_BEGIN or @ref EVENT_TOUCH_MOVE event type for this
         * finger id.
         * Populates the window id in @ref Event::windowId, finger id in @ref Event::data::touch::fingerId, normalized coordinates in
         * @ref Event::data::touch::x and @ref Event::data::touch::y and the delta values in @ref Event::data::touch::deltaX and @ref Event::data::touch::deltaY. 
         */
        EVENT_TOUCH_MOVE,

        /**
         * @brief Touch finger lifted from screen.
         *
         * Triggered when a touch finger has been lifted from the screen.
         * This event type uses normalized coordinates from -1.0f to 1.0f.
         * If the normalized coordinate represents the x value, -1.0f is the left edge of the screen, and 1.0f is the
         * right edge, else if the normalized coordinate represents the y value, -1.0f is the top edge, and 1.0f is the bottom edge.
         * The @ref Event::data::touch::deltaX and @ref Event::data::touch::deltaY values represent the amount of how much the normalized
         * position of the touch finger has changed since the last @ref EVENT_TOUCH_BEGIN or @ref EVENT_TOUCH_MOVE event type for this
         * finger id.
         * Populates the window id in @ref Event::windowId, finger id in @ref Event::data::touch::fingerId, normalized coordinates in
         * @ref Event::data::touch::x and @ref Event::data::touch::y and the delta values in @ref Event::data::touch::deltaX and @ref Event::data::touch::deltaY. 
         */
        EVENT_TOUCH_END,

        /**
         * @brief Device orientation changed.
         *
         * Triggered when the device's display orientation has changed.
         * This event can only be triggered when the platform supports different orientations,
         * the device's auto-rotate feature is turned on and the bitmask of allowed orientations is
         * a bitmask of more than one orientation.
         * To check platform orientations compatilibity, look at @ref PlatformCapability::fixedDefaultOrientation.
         * If the value is false, the platform supports different orintations. 
         * Populates the new orientation in @ref Event::data::orientation.
         *
         * @see Orientations
         * @see PlatformCapability
         */
        EVENT_DEVICE_ORIENTATION_CHANGED,
        
        /**
         * @brief Window unfocused.
         *
         * Triggered when the window becomes unfocused.
         * This event can only be triggered on desktop devices @ref DeviceType::DEVICE_DESKTOP.
         * To check the current device type, look at @ref PlatformCapability::deviceType.
         * Populates the window id in @ref Event::windowId.
         *
         * @see PlatformCapability
         * @see DeviceType
         */
        EVENT_PLATFORM_DESKTOP_UNFOCUSED,

        /**
         * @brief Window focused.
         *
         * Triggered when the window becomes focused.
         * This event can only be triggered on desktop devices @ref DeviceType::DEVICE_DESKTOP.
         * To check the current device type, look at @ref PlatformCapability::deviceType.
         * Populates the window id in @ref Event::windowId.
         *
         * @see PlatformCapability
         * @see DeviceType
         */
        EVENT_PLATFORM_DESKTOP_FOCUSED,

        /**
         * @brief Window will enter background.
         *
         * Triggered when the window will enter background.
         * This event can only be triggered on mobile devices @ref DeviceType::DEVICE_MOBILE.
         * To check the current device type, look at @ref PlatformCapability::deviceType.
         *
         * @see PlatformCapability
         * @see DeviceType
         */
        EVENT_PLATFORM_MOBILE_WILL_ENTER_BACKGROUND,

        /** 
         * @brief Window will enter foreground.
         *
         * Triggered when the window will enter foreground.
         * This event can only be triggered on mobile devices @ref DeviceType::DEVICE_MOBILE.
         * To check the current device type, look at @ref PlatformCapability::deviceType.
         *
         * @see PlatformCapability
         * @see DeviceType
         */
        EVENT_PLATFORM_MOBILE_WILL_ENTER_FOREGROUND,

        /**
         * @brief Low memory warning.
         *
         * Triggered when the device is running low on memory.
         * Once this event is recieved, it's recommended to free unused or low-priority memory
         * in order to avoid the OS killing the app.
         * This event can only be triggered on mobile devices @ref DeviceType::DEVICE_MOBILE.
         * To check the current device type, look at @ref PlatformCapability::deviceType.
         *
         * @see PlatformCapability
         * @see DeviceType
         */
        EVENT_PLATFORM_MOBILE_LOW_MEMORY
    };

    /**
     * @brief Window event.
     *
     * @see Window
     * @see Window::event_poll()
     * @see EventType
     * @see Gamepad
     * @see GamepadButton
     * @see GamepadAxis
     * @see KeyCode
     * @see DisplayOrientations
     * @see FoldingState
     */
    struct Event {
        EventType type = EVENT_NONE; ///< Type of event
        WindowID windowId = 0; ///< ID of the window this event occured on (0 for global/system events).

        union {
            struct {
                /**
                 * @brief Window width in logical pixels.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_WINDOW_RESIZED
                 */
                unsigned int width;

                /**
                 * @brief Window height in logical pixels.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_WINDOW_RESIZED
                 */
                unsigned int height;
            } window;

            struct {
                /**
                 * @brief Gamepad id.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_PAD_ADDED
                 * - @ref EventType::EVENT_PAD_REMOVED
                 * - @ref EventType::EVENT_PAD_BUTTON_DOWN
                 * - @ref EventType::EVENT_PAD_BUTTON_UP
                 * - @ref EventType::EVENT_PAD_AXIS_MOTION
                 */
                int id;

                /**
                 * @brief Gamepad button.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_PAD_BUTTON_DOWN
                 * - @ref EventType::EVENT_PAD_BUTTON_UP
                 */
                GamepadButton button;

                /**
                 * @brief Type of gamepad axis.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_PAD_AXIS_MOTION
                 */
                GamepadAxis axis;

                /**
                 * @brief Gamepad axis value.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_PAD_AXIS_MOTION
                 */
                float axisValue;
            } pad;

            struct  {
                /**
                 * @brief Keyboard key code.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_KEY_DOWN
                 * - @ref EventType::EVENT_KEY_UP
                 */
                KeyCode code;

                /**
                 * @brief Keyboard key repeat flag.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_KEY_DOWN
                 */
                bool isRepeat;
            } key;

            /**
             * @brief Display orientation.
             *
             * Populated for:
             * - @ref EventType::EVENT_DEVICE_ORIENTATION_CHANGED
             */
            DisplayOrientations orientation;

            struct {
                /**
                 * @brief Finger id.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_TOUCH_BEGIN
                 * - @ref EventType::EVENT_TOUCH_MOVE
                 * - @ref EventType::EVENT_TOUCH_END
                 */
                unsigned int fingerId;

                /**
                 * @brief Finger touch normalized X coordinate.
                 *
                 * The coordinate is normalized as follows:
                 * -1.0f is the left edge.
                 *  1.0f is the right edge.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_TOUCH_BEGIN
                 * - @ref EventType::EVENT_TOUCH_MOVE
                 * - @ref EventType::EVENT_TOUCH_END
                 */
                float x;

                /**
                 * @brief Finger touch normalized Y coordinate.
                 *
                 * The coordinate is normalized as follows:
                 * -1.0f is the left edge.
                 *  1.0f is the right edge.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_TOUCH_BEGIN
                 * - @ref EventType::EVENT_TOUCH_MOVE
                 * - @ref EventType::EVENT_TOUCH_END
                 */
                float y;

                /**
                 * @brief Finger touch normalized delta X coordinate.
                 *
                 * This value represents how the X value changed since the last @ref EventType::EVENT_TOUCH_BEGIN
                 * or @ref EventType::EVENT_TOUCH_MOVE event for this finger id.
                 * The coordinate is normalized as follows:
                 * -1.0f is the left edge.
                 *  1.0f is the right edge.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_TOUCH_MOVE
                 */
                float deltaX;

                /**
                 * @brief Finger touch normalized delta Y coordinate.
                 *
                 * This value represents how the Y value changed since the last @ref EventType::EVENT_TOUCH_BEGIN
                 * or @ref EventType::EVENT_TOUCH_MOVE event for this finger id.
                 * The coordinate is normalized as follows:
                 * -1.0f is the left edge.
                 *  1.0f is the right edge.
                 *
                 * Populated for:
                 * - @ref EventType::EVENT_TOUCH_MOVE
                 */
                float deltaY;
            } touch;
        } data;
    };

    /**
     * Main class interfaces.
     */

    struct Application {
        InternalApplication* internal = nullptr;

        /**
         * @brief Initializes the application and underlying platform subsystems.
         *
         * Initializes the platform subsystems (such as windowing, events, and gamepad input).
         * Must be created before creating any windows or polling events.
         *
         * @warning Always verify construction success using @ref valid(). In case of failure,
         * call @ref error_get() for error diagnostics and discard the object.
         *
         * @see valid()
         * @see ~Application()
         * @see event_poll()
         * @see capabilities_get()
         */
        Application();

        /**
         * @brief Terminates the application and underlying platform subsystems.
         *
         * Closes platform subsystems and releases all associated resources.
         *
         * @see Application()
         */
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        Application(Application&&);
        Application& operator=(Application&&);

        /**
         * @brief Checks if the application subsystem is valid and initialized.
         *
         * @return True if initialized successfully, false otherwise.
         *
         * @see Application()
         */
        bool valid() const;

        /**
         * @brief Polls the next event from the application event queue.
         *
         * Retrieves pending events from the platform's event queue and populates @p event.
         * For window-specific events (e.g., @ref EventType::EVENT_WINDOW_RESIZED, @ref EventType::EVENT_KEY_DOWN,
         * @ref EventType::EVENT_TOUCH_BEGIN), @ref Event::windowId contains the corresponding window's ID.
         * For system-wide/global events (e.g., @ref EventType::EVENT_QUIT, @ref EventType::EVENT_PAD_ADDED),
         * @ref Event::windowId is set to 0.
         *
         * @param[out] event Reference to an @ref Event the new event will be copied into. 
         *
         * @pre The application must be valid (@ref valid() == true).
         *
         * @return True if an event has been found and assigned, false if the end of the queue has been reached or invalid application.
         *
         * @see Event
         * @see WindowID
         * @see valid()
         */
        bool event_poll(Event& event) const;

        /**
         * @brief Gets the platform's capabilities.
         *
         * Gets the platform capabilities for the current platform and device.
         *
         * @pre The application must be valid (@ref valid() == true).
         *
         * @return Const reference to the platform's @ref PlatformCapabilities struct.
         *
         * @see PlatformCapabilities
         * @see valid()
         */
        const PlatformCapabilities& capabilities_get() const;
    };

    struct Window {
        InternalWindow* internal = nullptr;

        /**
         * @brief Creates a new application window with specified properties
         *
         * Initializes a new native OS window or connects to the platform framebuffer.
         *
         * @param[in] app                  Reference to the active @ref Application instance.
         * @param[in] title                UTF-8 window title string.
         * @param[in] width                Requested width in logical pixels.
         * @param[in] height               Requested height in logical pixels.
         * @param[in] flags                Bitmask combination of @ref WindowFlags.
         * @param[in] allowedOrientations  Bitmask combination of @ref DisplayOrientations flags.
         *
         * @pre The @p app must be valid (@ref Application::valid() == true).
         * @pre The @p title pointer must not be null.
         *
         * @note Sizing behavior is dictated by @ref PlatformCapabilities::sizeManagementType.
         * On platforms with a managed framebuffers, @p width and @p height are clamped to
         * an OS-provided size.
         * @note If @ref PlatformCapabilities::windowTitlesSupported is false, the @p title argument
         * is safely ignored.
         * @note On platforms, which have @ref PlatformCapabilities::fixedDefaultOrientation as false,
         * the allowedOrientations parameter controls auto-rotate allowed orientation, otherwise this
         * is safely ignored.
         *
         * @warning If @ref PlatformCapabilities::supportsMultipleWindows is false and another window
         * is currently valid, creation will fail and @ref valid() will return false.
         * @warning Always verify construction success using @ref valid(). In case of failure,
         * call @ref error_get() for error diagnostics and discard the object.
         *
         * @see Application
         * @see PlatformCapabilities
         * @see DisplayOrientations
         * @see SizeManagementType
         * @see valid()
         * @see error_get()
         * @see ~Window()
         * @see nwh_get()
         */
        Window(
            const Application& app,
            const char* title,
            unsigned int width,
            unsigned int height,
            unsigned int flags = WINDOW_FLAG_NONE,
            unsigned int allowedOrientations = ORIENTATION_LANDSCAPE
        );

        /**
         * @brief Destroys the window and releases its resources.
         *
         * Closes the native OS window handles and frees associated internal memory.
         *
         * @note If the window is invalid or in an moved-from state, this operation safely no-ops.
         *
         * @see Window()
         */
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&);
        Window& operator=(Window&&);

        /**
         * @brief Checks if the window is valid and initialized.
         *
         * Checks if the window is valid, created and the underlying internal resources are initialized.
         * This function should be called after initial window creation to verify if the creation was
         * successful.
         *
         * @return True if the window has a valid native handle and is ready to be used, false otherwise.
         *
         * @see Window()
         */
        bool valid() const;

        /**
         * @brief Gets the window's specific id.
         *
         * The @ref WindowID is specific per-window and set on creation.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return The window's id, or 0 if the window is invalid.
         *
         * @see valid()
         */
        WindowID id_get() const;

        /**
         * @brief Gets the window's native window handle.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return Pointer to the window's native window handle, or nullptr if unsupported or invalid.
         *
         * @note Check @ref PlatformCapabilities::nativeWindowHandleSupported to verify if native
         * window handles are supported, if they're not, a null pointer will be returned.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see PlatformCapabilities
         * @see valid()
         * @see error_get()
         */
        void* nwh_get() const;

        /**
         * @brief Gets the title of the window.
         *
         * Gets the current title of the window in the native OS window decoration.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return Pointer to a null-terminated UTF-8 title string, or nullptr if unsupported or invalid.
         *
         * @note Check @ref PlatformCapabilities::windowTitlesSupported to verify if window titles are
         * supported, if they're not, a null pointer will be returned.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see PlatformCapabilities
         * @see valid()
         * @see error_get()
         * @see title_set()
         */
        const char* title_get() const;

        /**
         * @brief Sets the title of the window.
         * 
         * Sets the title of the window in the native OS window decoration.
         *
         * @param[in] title UTF-8 string representing the new window title.
         *
         * @pre The window must be valid (@ref valid() == true).
         * @pre The @p title pointer must not be null.
         *
         * @return True on success, false if unsupported or invalid.
         *
         * @note Check @ref PlatformCapabilities::windowTitlesSupported to determine if the current
         * platform permits custom window titles. If this value is false, this function will always
         * return false.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see PlatformCapabilities
         * @see valid()
         * @see error_get()
         * @see title_get()
         */
        bool title_set(const char* title);

        /**
         * @brief Gets the window's logical dimensions.
         *
         * @param[out] outWidth  The window's width in logical pixels.
         * @param[out] outHeight The window's height in logical pixels.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return True on success, false if failed or window is invalid.
         *
         * @note On platforms with a managed window size, this function should be used to get the initial
         * managed dimensions.
         * @note If the return value is false, the values weren't changed.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         * @see size_set()
         * @see size_min_get()
         * @see size_min_set()
         * @see size_max_get()
         * @see size_max_set()
         * @see hidpi_scale_get()
         * @see safe_area_insets_get()
         */
        bool size_get(unsigned int& outWidth, unsigned int& outHeight) const;

        /**
         * @brief Sets the window's logical dimensions.
         *
         * @param[in] width  Requested new width in logical pixels.
         * @param[in] height Requested new height in logical pixels.
         *
         * @pre The window must be valid (@ref valid() == true).
         * @pre @p width and @p height must be greater than 0.
         *
         * @return True if resize completed successfully, false on failure or if resizing is unsupported.
         *
         * @note Refer to @ref PlatformCapabilities::sizeManagementType to check if resizing is supported.
         * This function will return false and the resize will fail if the value is other than
         * @ref SizeManagementType::SIZE_FREE_RESIZABLE.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see PlatformCapabilities
         * @see SizeManagementType
         * @see valid()
         * @see error_get()
         * @see size_get()
         * @see size_min_get()
         * @see size_min_set()
         * @see size_max_get()
         * @see size_max_set()
         * @see aspect_ratio_get()
         * @see hidpi_scale_get()
         * @see safe_area_insets_get()
         */
        bool size_set(unsigned int width, unsigned height);

        /**
         * @brief Gets the window's minimum logical dimensions.
         *
         * @param[out] outWidth  The window's minimum width in logical pixels.
         * @param[out] outHeight The window's minimum height in logical pixels.
         *
         * @pre The window must be valid (@ref valid() == true).
         * @pre The window must have the WindowFlags::WINDOW_FLAG_RESIZABLE flag.
         *
         * @return True if retrieving the dimensions was successful, false on failure or if minimum
         * dimensions are unsupported.
         *
         * @note Refer to @ref PlatformCapabilities::sizeManagementType to check if minimum
         * dimensions are supported. If this value is anything other than
         * @ref SizeManagementType::SIZE_FREE_RESIZABLE, this function will return false.
         * @note If the window doesn't have the @ref WindowFlags::WINDOW_FLAG_RESIZABLE flag, this
         * function will return false.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         * @note If the window is invalid or the operation failed, the values won't be changed.
         *
         * @see PlatformCapabilities
         * @see SizeManagementType
         * @see WindowFlags
         * @see valid()
         * @see error_get()
         * @see size_min_set()
         * @see size_get()
         * @see size_set()
         * @see size_max_get()
         * @see size_max_set()
         * @see aspect_ratio_get()
         * @see hidpi_scale_get()
         * @see safe_area_insets_get()
         */
        bool size_min_get(unsigned int& outWidth, unsigned int& outHeight) const;

        /**
         * @brief Sets the window's minimum logical dimensions.
         *
         * @param[in] width  Requested minimum width in logical pixels.
         * @param[in] height Requested minimum height in logical pixels.
         *
         * @pre The window must be valid (@ref valid() == true).
         * @pre The window must have the WindowFlags::WINDOW_FLAG_RESIZABLE flag.
         * @pre @p width and @p height must be greater than 0.
         *
         * @return True if setting the dimensions was successful, false on failure or if minimum
         * dimensions are unsupported.
         *
         * @note Refer to @ref PlatformCapabilities::sizeManagementType to check if minimum
         * dimensions are supported. If this value is anything other than
         * @ref SizeManagementType::SIZE_FREE_RESIZABLE, this function will return false.
         * @note If the window doesn't have the @ref WindowFlags::WINDOW_FLAG_RESIZABLE flag, this
         * function will return false.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see PlatformCapabilities
         * @see SizeManagementType
         * @see WindowFlags
         * @see valid()
         * @see error_get()
         * @see size_min_get()
         * @see size_get()
         * @see size_set()
         * @see size_max_get()
         * @see size_max_set()
         * @see aspect_ratio_get()
         * @see hidpi_scale_get()
         * @see safe_area_insets_get()
         */
        bool size_min_set(unsigned int width, unsigned int height);

        /**
         * @brief Gets the window's maximum logical dimensions.
         *
         * @param[out] outWidth  The window's maximum width in logical pixels.
         * @param[out] outHeight The window's maximum height in logical pixels.
         *
         * @pre The window must be valid (@ref valid() == true).
         * @pre The window must have the WindowFlags::WINDOW_FLAG_RESIZABLE flag.
         *
         * @return True if retrieving the dimensions was successful, false on failure or if maximum
         * dimensions are unsupported.
         *
         * @note Refer to @ref PlatformCapabilities::sizeManagementType to check if maximum
         * dimensions are supported. If this value is anything other than
         * @ref SizeManagementType::SIZE_FREE_RESIZABLE, this function will return false.
         * @note If the window doesn't have the @ref WindowFlags::WINDOW_FLAG_RESIZABLE flag, this
         * function will return false.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         * @note If the window is invalid or the operation failed, the values won't be changed.
         *
         * @see PlatformCapabilities
         * @see SizeManagementType
         * @see WindowFlags
         * @see valid()
         * @see error_get()
         * @see size_max_set()
         * @see size_get()
         * @see size_set()
         * @see size_min_get()
         * @see size_min_set()
         * @see aspect_ratio_get()
         * @see hidpi_scale_get()
         * @see safe_area_insets_get()
         */
        bool size_max_get(unsigned int& outWidth, unsigned int& outHeight) const;

        /**
         * @brief Sets the window's maximum logical dimensions.
         *
         * @param[in] width  Requested maximum width in logical pixels.
         * @param[in] height Requested maximum height in logical pixels.
         *
         * @pre The window must be valid (@ref valid() == true).
         * @pre The window must have the WindowFlags::WINDOW_FLAG_RESIZABLE flag.
         * @pre @p width and @p height must be greater than 0.
         *
         * @return True if setting the dimensions was successful, false on failure or if maximum
         * dimensions are unsupported.
         *
         * @note Refer to @ref PlatformCapabilities::sizeManagementType to check if maximum
         * dimensions are supported. If this value is anything other than
         * @ref SizeManagementType::SIZE_FREE_RESIZABLE, this function will return false.
         * @note If the window doesn't have the @ref WindowFlags::WINDOW_FLAG_RESIZABLE flag, this
         * function will return false.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see PlatformCapabilities
         * @see SizeManagementType
         * @see WindowFlags
         * @see valid()
         * @see error_get()
         * @see size_max_get()
         * @see size_get()
         * @see size_set()
         * @see size_min_get()
         * @see size_min_set()
         * @see aspect_ratio_get()
         * @see hidpi_scale_get()
         * @see safe_area_insets_get()
         */
        bool size_max_set(unsigned int width, unsigned int height);

        /**
         * @brief Gets the window's aspect ratio.
         *
         * Calculates the window's aspect ratio from it's current width and height.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return The window's aspect ratio, or 0.0f if the operation failed or invalid.
         * 
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         * @see size_get()
         * @see size_set()
         * @see size_min_get()
         * @see size_min_set()
         * @see size_max_get()
         * @see size_max_set()
         * @see hidpi_scale_get()
         * @see safe_area_insets_get()
         */
        float aspect_ratio_get() const;

        /**
         * @brief Gets the window's content scale.
         *
         * Gets how many physical pixels the window covers per logical pixel, which is the scale
         * content that isn't resolution-independent (text and UI bitmaps, for example) should
         * be scaled by in order to match the display's pixel density.
         * For example on an Apple display this works as follows:
         * - 1.0f - Standard resolution,
         * - 2.0f - "Retina" resolution(HiDPI, 2x as much resolution as standard).
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return The window's content scale, 0.0f indicates failure.
         *
         * @note This reflects the display's pixel density on every platform and device type.
         * @note The value changes over time, @ref EventType::EVENT_WINDOW_HIDPI_SCALE_CHANGED
         * is triggered whenever it does.
         * @note The renderer doesn't automatically render at this density, it has to be passed
         * to the renderer in order to render at the window's physical resolution, refer to
         * `RendererConfig::contentScale` and `Renderer::size_get()`.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see EventType::EVENT_WINDOW_HIDPI_SCALE_CHANGED
         * @see valid()
         * @see error_get()
         * @see size_get()
         * @see size_set()
         * @see size_min_get()
         * @see size_min_set()
         * @see size_max_get()
         * @see size_max_set()
         * @see aspect_ratio_get()
         * @see safe_area_insets_get()
         */
        float hidpi_scale_get() const;

        /**
         * @brief Gets the current safe area insets.
         *
         * Gets the safe area insets for the current window dimensions and orientation.
         * This is dependent on the window dimensions and display orientation, so it
         * should be called when any one of these change.
         * The insets are in logical pixels, rotated to match the current display
         * orientation.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return The window's current safe area insets, or 0.0f in every field if invalid.
         *
         * @see SafeAreaInsets
         * @see DisplayOrientations
         * @see valid()
         * @see size_get()
         * @see size_set()
         * @see size_min_get()
         * @see size_min_set()
         * @see size_max_get()
         * @see size_max_set()
         * @see aspect_ratio_get()
         * @see hidpi_scale_get()
         */
        SafeAreaInsets safe_area_insets_get() const;

        /**
         * @brief Gets the window's current flags.
         *
         * Gets the current bitmask of flags for the window.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return The window's current flags, or @ref WindowFlags::WINDOW_FLAG_NONE if window is invalid.
         *
         * @see WindowFlags
         * @see valid()
         * @see flags_set()
         */
        unsigned int flags_get() const;

        /**
         * @brief Sets the window's flags.
         *
         * Updates the window's flags bitmask value.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @param[in] flags Requested bitmask of @ref WindowFlags to be applied.
         *
         * @return True if the flags were updated successfully, false if at least one of the operations failed, unsupported or invalid window.
         *
         * @note This function is only supported for @ref DeviceType::DEVICE_DESKTOP, this can be checked in @ref PlatformCapabilities.
         * On non-desktop device types, this always results in a false return value.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see WindowFlags
         * @see PlatformCapabilities
         * @see DeviceType
         * @see valid()
         * @see flags_set()
         * @see capabilities_get()
         */
        bool flags_set(unsigned int flags);

        /**
         * @brief Gets the currently allowed display orientations.
         *
         * Gets a bitmask of the currently allowed display orientations for auto-rotate.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return The display's currently allowed display orientations, or
         * @ref DisplayOrientations::ORIENTATION_NONE if unsupported or invalid window.
         *
         * @note Check (@ref PlatformCapabilities::fixedDefaultOrientation == false), to
         * verify if display orientations are supported, if they're not,
         * @ref DisplayOrientations::ORIENTATION_NONE will be returned.
         *
         * @see DisplayOrientations
         * @see PlatformCapabilities
         * @see valid()
         * @see orientations_set()
         */
        unsigned int orientations_get() const;

        /**
         * @brief Sets the currently allowed display orientations.
         *
         * Sets a bitmask value of the currently allowed display orientations for auto-rotate.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @param[in] orientations Bitmask value of requested allowed display orientations
         *
         * @return True on success, false if unsupported or invalid window.
         *
         * @note Check (@ref PlatformCapabilities::fixedDefaultOrientation == false), to
         * verify if display orientations are supported, if they're not, false will be returned.
         * @note iPadOS devices always have the allowed orientations as @ref DisplayOrientations::ORIENTATION_ALL
         * regardless of @p orientations and calls to this function are ignored, unless
         * the @ref WindowFlags::WINDOW_FLAG_STRICT_ORIENTATIONS flags was set during window creation.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see DisplayOrientations
         * @see PlatformCapabilities
         * @see valid()
         * @see orientations_get()
         */
        bool orientations_set(unsigned int orientations);

        /**
         * @brief Gets the current display orientation.
         *
         * @pre The window must be valid (@ref valid() == true).
         *
         * @return The display's current display orientation, or
         * @ref DisplayOrientations::ORIENTATION_NONE if unsupported or invalid window.
         *
         * @note Check (@ref PlatformCapabilities::fixedDefaultOrientation == false), to
         * verify if display orientations are supported, if they're not,
         * @ref DisplayOrientations::ORIENTATION_NONE will be returned.
         *
         * @see DisplayOrientations
         * @see PlatformCapabilities
         * @see valid()
         * @see orientations_set()
         */
        unsigned int orientation_get() const;
    };

    struct Gamepad {
        InternalGamepad* internal = nullptr;
        
        /**
         * @brief Connects to a gamepad using a gamepad id.
         *
         * @pre The @p app must be valid (@ref Application::valid() == true).
         * @pre The gamepad at the specified id must be available to connect to.
         *
         * @param[in] app Reference to the active @ref Application instance.
         * @param[in] id  Gamepad id of the requested gamepad.
         *
         * @warning Always verify construction success using @ref valid(). In case of failure,
         * call @ref error_get() for error diagnostics and discard the object.
         *
         * @see Application
         * @see ~Gamepad()
         * @see valid()
         * @see error_get()
         */
        Gamepad(const Application& app, int id);

        /**
         * @brief Disconnects from a gamepad.
         *
         * @note If the gamepad is invalid, this function will be safely ignored.
         *
         * @see Gamepad()
         */
        ~Gamepad();

        Gamepad(const Gamepad&) = delete;
        Gamepad& operator=(const Gamepad&) = delete;

        Gamepad(Gamepad&&);
        Gamepad& operator=(Gamepad&&);

        /**
         * @brief Checks if the gamepad is valid and connected.
         *
         * Checks if the gamepad is valid, created and the underlying internal resources are initialized.
         * This function should be called after creating the gamepad object to verify if the creation was
         * successful.
         *
         * @return True if the gamepad is ready to be used, false otherwise.
         *
         * @see Gamepad()
         */
        bool valid() const;

        /**
         * @brief Gets the human-readable product name of the gamepad.
         *
         * @pre The gamepad must be valid (@ref valid() == true).
         *
         * @return Pointer to a UTF8 string (e.g., "DualSense Wireless Controller"),
         * or nullptr if the gamepad is invalid.
         *
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         */
        const char* name_get() const;

        /**
         * @brief Gets the gamepad id.
         *
         * @pre The gamepad must be valid (@ref valid() == true).
         *
         * @return Integer ID matching the @ref Event::data::pad::id used to connect to this gamepad,
         * or -1 on failure or if the gamepad is invalid.
         *
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         */
        int id_get() const;

        /**
         * @brief Gets the recognized type/model of the gamepad.
         *
         * @pre The gamepad must be valid (@ref valid() == true).
         *
         * @return @ref GamepadType enum representing the controller hardware layout, or @ref GamepadType::PAD_TYPE_UNKNOWN
         * on failure or if the gamepad is invalid.
         *
         * @see valid()
         * @see GamepadType
         */
        GamepadType type_get() const;

        /**
         * @brief Triggers standard dual-motor haptic rumble vibration.
         *
         * @param[in] lowFreq    Low-frequency (heavy/bass rumble) motor intensity, from 0 to 65535 (0xFFFF).
         * @param[in] highFreq   High-frequency (sharp/light rumble) motor intensity, from 0 to 65535 (0xFFFF).
         * @param[in] durationMs Duration of the rumble vibration in miliseconds.
         *
         * @pre The gamepad must be valid(@ref valid() == true).
         *
         * @return True if the rumble effect was sent successfully, false if unsupported or failed.
         *
         * @note On failure or if the controller lacks rumble motors, an error is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         */
        bool rumble_set(uint16_t lowFreq, uint16_t highFreq, uint32_t durationMs) const;

        /**
         * @brief Triggers independent trigger-motor haptic feedback (impulse triggers).
         *
         * @param[in] leftTrigger  Left trigger motor intensity, from 0 to 65535 (0xFFFF).
         * @param[in] rightTrigger Right trigger motor intensity, from 0 to 65535 (0xFFFF).
         * @param[in] durationMs   Duration of the rumble vibration in miliseconds.
         *
         * @pre The gamepad must be valid(@ref valid() == true).
         *
         * @return True if the trigger rumble was sent successfully, false if unsupported or failed.
         *
         * @note On gamepad controllers without dedicated trigger motors, the average of @ref leftTrigger and @ref rightTrigger
         * will be sent to the standard dual-motor high and low frequency haptic rumble vibration instead.
         *
         * @note On failure or if the controller lacks rumble motors, an error is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         */
        bool rumble_set_trigger(uint16_t leftTrigger, uint16_t rightTrigger, uint32_t durationMs) const;
    };
}
