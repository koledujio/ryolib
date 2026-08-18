#include "../private.h"

#include <algorithm>
#include <cstdio>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"

namespace ryo {
    ResourcePool<_InternalWindow, MAX_WINDOWS> _window_pool;

    Window window_create(const char* title, unsigned int width, unsigned int height) {
        RYO_ENSURE(title != nullptr, {}, "Null title string pointer provided");

        if (_window_pool.active_count >= MAX_WINDOWS) {
            error_set("Maximum number of windows reached");
            return {};
        }

        if (!sdl_initialized) {
            if (!_sdl_initialize()) {
                error_set("Failed to initialize SDL: %s", SDL_GetError());
                return {};
            }
        }

        SDL_Window* window = SDL_CreateWindow(title, static_cast<int>(width), static_cast<int>(height), 0);

        if (!window) {
            error_set("Failed to create SDL window: %s", SDL_GetError());

            if (_window_pool.active_count == 0) {
                _sdl_quit();
            }
            return { 0 };
        }

        INDEX idx = _window_pool.allocate_index();
        GENID gen = _window_pool.generations[idx];

        RYO_ASSERT(idx < MAX_WINDOWS && idx != 0, { 0 }, "Resource pool returned invalid index");

        _window_pool.items[idx].sdlWindow = window;

        return Window::pack(idx, gen);
    }

    void window_destroy(Window window) {
        RYO_ENSURE_VOID(window_is_valid(window), "Invalid window handle");

        SDL_DestroyWindow(_window_pool.items[window.index()].sdlWindow);

        if (_window_pool.active_count == 0) {
            _sdl_quit();
        }
    }

    bool window_is_valid(Window window) {
        INDEX idx = window.index();
        GENID gen = window.gen();

        if (idx >= MAX_WINDOWS || idx == 0)
            return false;

        if (_window_pool.items[idx].sdlWindow == nullptr)
            return false;

        return _window_pool.is_valid(idx, gen);
    }

    bool window_poll_event(Window window, Event& event) {
        RYO_ENSURE(window_is_valid(window), false, "Invalid window handle");

        SDL_Event& sdlEvent = _window_pool.items[window.index()].event;

        if (!SDL_PollEvent(&sdlEvent))
            return false;

        event = Event{};

        switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:
                event.type = EVENT_QUIT;
                break;
            
            case SDL_EVENT_GAMEPAD_ADDED:
                event.type = EVENT_PAD_ADDED;
                event.data.pad.id = (GamepadId)sdlEvent.gdevice.which;
                event.data.pad.type = _gamepad_pool.items[event.data.pad.id].type;
                break;
            
            case SDL_EVENT_GAMEPAD_REMOVED:
                event.type = EVENT_PAD_REMOVED;
                event.data.pad.id = (GamepadId)sdlEvent.gdevice.which;
                event.data.pad.type = _gamepad_pool.items[event.data.pad.id].type;
                break;
            
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                event.type = EVENT_PAD_BUTTON_DOWN;
                event.data.pad.id = (GamepadId)sdlEvent.gdevice.which;
                event.data.pad.type = _gamepad_pool.items[event.data.pad.id].type;
                event.data.pad.button = _gamepad_get_ryo_button(sdlEvent.gbutton.button);
                break;
            
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                event.type = EVENT_PAD_BUTTON_UP;
                event.data.pad.id = (GamepadId)sdlEvent.gdevice.which;
                event.data.pad.type = _gamepad_pool.items[event.data.pad.id].type;
                event.data.pad.button = _gamepad_get_ryo_button(sdlEvent.gbutton.button);
                break;
            
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                event.type = EVENT_PAD_AXIS_MOTION;
                event.data.pad.id = (GamepadId)sdlEvent.gaxis.which;
                event.data.pad.type = _gamepad_pool.items[event.data.pad.id].type;
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

    void* window_get_nwh(Window window) {
        RYO_ENSURE(window_is_valid(window), nullptr, "Invalid window handle");

        SDL_PropertiesID props = SDL_GetWindowProperties(_window_pool.items[window.index()].sdlWindow);
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