#include "../private.h"

#include <algorithm>
#include <cstdio>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"

namespace ryo {
    uint32_t window_count = 0;

    Window::Window(const char* title, unsigned int width, unsigned int height) {
        RYO_ENSURE_VOID(title != nullptr, "Null title string pointer provided");

        if (window_count == 4294967295) {
            error_set("Maximum number of windows reached");
            return;
        }

        if (!sdl_initialized) {
            if (!_sdl_initialize()) {
                error_set("Failed to initialize SDL: %s", SDL_GetError());
                return;
            }
        }

        SDL_Window* window = SDL_CreateWindow(title, static_cast<int>(width), static_cast<int>(height), 0);

        if (!window) {
            error_set("Failed to create SDL window: %s", SDL_GetError());

            if (window_count == 0) {
                _sdl_quit();
            }
            return;
        }

        InternalWindow* intern = new InternalWindow;
        intern->sdlWindow = window;

        internal = intern;

        window_count++;
    }

    Window::~Window() {
        if (!internal)
            return;

        if (internal->sdlWindow)
            SDL_DestroyWindow(internal->sdlWindow);

        delete internal;

        window_count--;

        if (window_count == 0) {
            _sdl_quit();
        }
    }

    Window::Window(Window&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Window& Window::operator=(Window&& other) {
        if (this == &other)
            return *this;

        if (other.valid())
            window_count--;

        if (internal) {
            if (internal->sdlWindow)
                SDL_DestroyWindow(internal->sdlWindow);

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Window::valid() const {
        return internal != nullptr && internal->sdlWindow != nullptr;
    }

    bool Window::event_poll(Event& event) {
        RYO_ENSURE(valid(), false, "Invalid window handle");

        SDL_Event& sdlEvent = internal->event;

        if (!SDL_PollEvent(&sdlEvent))
            return false;

        event = Event{};

        switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:
                event.type = EVENT_QUIT;
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
                event.data.pad.id = sdlEvent.gdevice.which;
                event.data.pad.button = _gamepad_get_ryo_button(sdlEvent.gbutton.button);
                break;
            
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                event.type = EVENT_PAD_BUTTON_UP;
                event.data.pad.id = sdlEvent.gdevice.which;
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
                event.data.key.code = _keyboard_get_ryo_key(sdlEvent.key.scancode);
                event.data.key.isRepeat = sdlEvent.key.repeat;
                break;
            
            case SDL_EVENT_KEY_UP:
                event.type = EVENT_KEY_UP;
                event.data.key.code = _keyboard_get_ryo_key(sdlEvent.key.scancode);
                event.data.key.isRepeat = sdlEvent.key.repeat;
                break;

            default:
                break;
        }

        return true;
    }

    void* Window::nwh_get() {
        RYO_ENSURE(valid(), nullptr, "Invalid window handle");

        SDL_PropertiesID props = SDL_GetWindowProperties(internal->sdlWindow);
        if (props == 0) {
            return nullptr;
        }

        #if defined(_WIN32)
        return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        #elif defined(__APPLE__)
        return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        #else
        log(LOG_TYPE_ERROR, "Window", "SDL", "Couldn't get a native window handle for an unsupported platform!");
        return nullptr;
        #endif
    }
}