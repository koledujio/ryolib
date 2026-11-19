#include "../private.h"

#include <algorithm>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"

namespace ryo {
    Application::Application() {
        if (!sdl_initialized) {
            if (!_sdl_initialize()) {
                error_set("Failed to initialize platform application: %s", SDL_GetError());
                return;
            }
        }

        internal = new InternalApplication;
    }

    Application::~Application() {
        if (!internal)
            return;

        delete internal;
        internal = nullptr;

        _sdl_quit();
    }

    Application::Application(Application&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Application& Application::operator=(Application&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            delete internal;
            _sdl_quit();
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Application::valid() const {
        return internal != nullptr;
    }

    bool Application::event_poll(Event& event) const {
        RYO_ENSURE(valid(), false, "Invalid application");

        SDL_Event sdlEvent;
        if (!SDL_PollEvent(&sdlEvent))
            return false;

        event = Event{};
        event.type = EVENT_NONE;
        event.windowId = 0;

        const PlatformCapabilities& capabilities = capabilities_get();

        switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:
                event.type = EVENT_QUIT;
                break;
            
            case SDL_EVENT_WINDOW_RESIZED:
                event.type = EVENT_WINDOW_RESIZED;
                event.windowId = static_cast<WindowID>(sdlEvent.window.windowID);
                event.data.window.width = static_cast<unsigned int>(sdlEvent.window.data1);
                event.data.window.height = static_cast<unsigned int>(sdlEvent.window.data2);
                break;
            
            case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
                event.type = EVENT_WINDOW_HIDPI_SCALE_CHANGED;
                event.windowId = static_cast<WindowID>(sdlEvent.window.windowID);
                break;
            
            case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
                event.type = EVENT_WINDOW_SAFE_AREA_CHANGED;
                event.windowId = static_cast<WindowID>(sdlEvent.window.windowID);
                break;
            
            case SDL_EVENT_GAMEPAD_ADDED:
                event.type = EVENT_PAD_ADDED;
                event.data.pad.id = sdlEvent.gdevice.which;
                break;
            
            case SDL_EVENT_GAMEPAD_REMOVED:
                event.type = EVENT_PAD_REMOVED;
                event.data.pad.id = sdlEvent.gdevice.which;
                break;
            
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                event.type = EVENT_PAD_BUTTON_DOWN;
                event.data.pad.id = sdlEvent.gbutton.which;
                event.data.pad.button = _gamepad_get_ryo_button(sdlEvent.gbutton.button);
                break;
            
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                event.type = EVENT_PAD_BUTTON_UP;
                event.data.pad.id = sdlEvent.gbutton.which;
                event.data.pad.button = _gamepad_get_ryo_button(sdlEvent.gbutton.button);
                break;
            
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                event.type = EVENT_PAD_AXIS_MOTION;
                event.data.pad.id = sdlEvent.gaxis.which;
                event.data.pad.axis = _gamepad_get_ryo_axis(sdlEvent.gaxis.axis);
                event.data.pad.axisValue = std::clamp(sdlEvent.gaxis.value / 32767.0f, -1.0f, 1.0f);
                break;
            
            case SDL_EVENT_KEY_DOWN:
                event.type = EVENT_KEY_DOWN;
                event.windowId = static_cast<WindowID>(sdlEvent.key.windowID);
                event.data.key.code = _keyboard_get_ryo_key(sdlEvent.key.scancode);
                event.data.key.isRepeat = sdlEvent.key.repeat;
                break;
            
            case SDL_EVENT_KEY_UP:
                event.type = EVENT_KEY_UP;
                event.windowId = static_cast<WindowID>(sdlEvent.key.windowID);
                event.data.key.code = _keyboard_get_ryo_key(sdlEvent.key.scancode);
                event.data.key.isRepeat = sdlEvent.key.repeat;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                event.type = EVENT_MOUSE_MOVE;
                event.windowId = static_cast<WindowID>(sdlEvent.motion.windowID);
                event.data.mouse.x = sdlEvent.motion.x;
                event.data.mouse.y = sdlEvent.motion.y;
                event.data.mouse.deltaX = sdlEvent.motion.xrel;
                event.data.mouse.deltaY = sdlEvent.motion.yrel;
                event.data.mouse.buttons = static_cast<unsigned int>(sdlEvent.motion.state);
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                event.type = sdlEvent.button.down ? EVENT_MOUSE_BUTTON_DOWN : EVENT_MOUSE_BUTTON_UP;
                event.windowId = static_cast<WindowID>(sdlEvent.button.windowID);
                event.data.mouse.x = sdlEvent.button.x;
                event.data.mouse.y = sdlEvent.button.y;
                event.data.mouse.button = _mouse_get_ryo_button(sdlEvent.button.button);
                event.data.mouse.clicks = sdlEvent.button.clicks;
                event.data.mouse.buttons = static_cast<unsigned int>(SDL_GetMouseState(nullptr, nullptr));
                break;

            case SDL_EVENT_MOUSE_WHEEL: {
                event.type = EVENT_MOUSE_WHEEL;
                event.windowId = static_cast<WindowID>(sdlEvent.wheel.windowID);
                event.data.mouse.x = sdlEvent.wheel.mouse_x;
                event.data.mouse.y = sdlEvent.wheel.mouse_y;
                float scrollSign = sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
                event.data.mouse.scrollX = sdlEvent.wheel.x * scrollSign;
                event.data.mouse.scrollY = -sdlEvent.wheel.y * scrollSign;
                break;
            }

            case SDL_EVENT_FINGER_DOWN:
                event.type = EVENT_TOUCH_BEGIN;
                event.windowId = static_cast<WindowID>(sdlEvent.tfinger.windowID);
                event.data.touch.fingerId = static_cast<unsigned int>(sdlEvent.tfinger.fingerID);
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                break;

            case SDL_EVENT_FINGER_UP:
                event.type = EVENT_TOUCH_END;
                event.windowId = static_cast<WindowID>(sdlEvent.tfinger.windowID);
                event.data.touch.fingerId = static_cast<unsigned int>(sdlEvent.tfinger.fingerID);
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                break;

            case SDL_EVENT_FINGER_MOTION:
                event.type = EVENT_TOUCH_MOVE;
                event.windowId = static_cast<WindowID>(sdlEvent.tfinger.windowID);
                event.data.touch.fingerId = static_cast<unsigned int>(sdlEvent.tfinger.fingerID);
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                event.data.touch.deltaX = sdlEvent.tfinger.dx * 2.0f;
                event.data.touch.deltaY = sdlEvent.tfinger.dy * 2.0f;
                break;

            case SDL_EVENT_FINGER_CANCELED:
                event.type = EVENT_TOUCH_END;
                event.windowId = static_cast<WindowID>(sdlEvent.tfinger.windowID);
                event.data.touch.fingerId = static_cast<unsigned int>(sdlEvent.tfinger.fingerID);
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                break;
            
            case SDL_EVENT_DISPLAY_ORIENTATION:
                if (!capabilities.fixedDefaultOrientation) {
                    event.type = EVENT_DEVICE_ORIENTATION_CHANGED;
                    switch (static_cast<SDL_DisplayOrientation>(sdlEvent.display.data1)) {
                        case SDL_ORIENTATION_LANDSCAPE:
                            event.data.orientation = ORIENTATION_LANDSCAPE;
                            break;
                        
                        case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
                            event.data.orientation = ORIENTATION_LANDSCAPE_FLIPPED;
                            break;
                        
                        case SDL_ORIENTATION_PORTRAIT:
                            event.data.orientation = ORIENTATION_PORTRAIT;
                            break;
                        
                        case SDL_ORIENTATION_PORTRAIT_FLIPPED:
                            event.data.orientation = ORIENTATION_PORTRAIT_FLIPPED;
                            break;
                        
                        default:
                            event.data.orientation = ORIENTATION_NONE;
                            break;
                    }
                }
                break;
            
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                if (capabilities.deviceType == DEVICE_DESKTOP) {
                    event.type = EVENT_PLATFORM_DESKTOP_FOCUSED;
                    event.windowId = static_cast<WindowID>(sdlEvent.window.windowID);
                }
                break;
            
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                if (capabilities.deviceType == DEVICE_DESKTOP) {
                    event.type = EVENT_PLATFORM_DESKTOP_UNFOCUSED;
                    event.windowId = static_cast<WindowID>(sdlEvent.window.windowID);
                }
                break;
            
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
                if (capabilities.deviceType == DEVICE_MOBILE)
                    event.type = EVENT_PLATFORM_MOBILE_WILL_ENTER_BACKGROUND;
                break;
            
            case SDL_EVENT_WILL_ENTER_FOREGROUND:
                if (capabilities.deviceType == DEVICE_MOBILE)
                    event.type = EVENT_PLATFORM_MOBILE_WILL_ENTER_FOREGROUND;
                break;
            
            case SDL_EVENT_LOW_MEMORY:
                if (capabilities.deviceType == DEVICE_MOBILE)
                    event.type = EVENT_PLATFORM_MOBILE_LOW_MEMORY;
                break;

            default:
                break;
        }

        return true;
    }

    const PlatformCapabilities& Application::capabilities_get() const {
        return _platform_get_capabilities();
    }
}
