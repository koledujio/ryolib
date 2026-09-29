#include "../private.h"

#include <algorithm>
#include <cstdio>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"

namespace ryo {
    uint32_t window_count = 0;

    Window::Window(
        const char* title,
        unsigned int width,
        unsigned int height,
        unsigned int flags,
        unsigned int allowedOrientations
    ) {
        RYO_ENSURE_VOID(title != nullptr, "Null title string pointer provided");

        PlatformCapabilities capabilities = _platform_get_capabilities();

        uint64_t sdlFlags = 0;

        if (capabilities.deviceType == DEVICE_DESKTOP) {
            if ((flags & WINDOW_FLAG_RESIZABLE) == WINDOW_FLAG_RESIZABLE)
                sdlFlags |= SDL_WINDOW_RESIZABLE;

            if ((flags & WINDOW_FLAG_FULLSCREEN) == WINDOW_FLAG_FULLSCREEN)
                sdlFlags |= SDL_WINDOW_FULLSCREEN;

            if ((flags & WINDOW_FLAG_BORDERLESS) == WINDOW_FLAG_BORDERLESS)
                sdlFlags |= SDL_WINDOW_BORDERLESS;

            if ((flags & WINDOW_FLAG_HIDPI) == WINDOW_FLAG_HIDPI)
                sdlFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
        }

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

        unsigned int newWidth = width;
        unsigned int newHeight = height;
        _platform_get_forced_resolution(newWidth, newHeight);

        SDL_Window* window = SDL_CreateWindow(title, static_cast<int>(newWidth), static_cast<int>(newHeight), sdlFlags);

        if (!window) {
            error_set("Failed to create SDL window: %s", SDL_GetError());

            if (window_count == 0) {
                _sdl_quit();
            }
            return;
        }

        InternalWindow* intern = new InternalWindow;
        intern->sdlWindow = window;
        intern->capabilities = capabilities;
        intern->title = capabilities.windowTitlesSupported ? title : nullptr;
        intern->flags = capabilities.deviceType == DEVICE_DESKTOP ? flags : WINDOW_FLAG_NONE;
        intern->orientations = capabilities.fixedDefaultOrientation ? ORIENTATION_NONE : allowedOrientations;

        internal = intern;

        if (!capabilities.fixedDefaultOrientation)
            orientations_set(allowedOrientations);

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
        RYO_ENSURE(valid(), false, "Invalid window");

        SDL_Event& sdlEvent = internal->event;

        if (!SDL_PollEvent(&sdlEvent))
            return false;

        event = Event{};
        event.type = EVENT_NONE;

        switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:
                event.type = EVENT_QUIT;
                break;
            
            case SDL_EVENT_WINDOW_RESIZED:
                event.type = EVENT_WINDOW_RESIZED;
                event.data.window.width = sdlEvent.window.data1;
                event.data.window.height = sdlEvent.window.data2;
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
            
            case SDL_EVENT_FINGER_DOWN:
                event.type = EVENT_TOUCH_BEGIN;
                event.data.touch.fingerId = sdlEvent.tfinger.fingerID;
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                break;

            case SDL_EVENT_FINGER_UP:
                event.type = EVENT_TOUCH_END;
                event.data.touch.fingerId = sdlEvent.tfinger.fingerID;
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                break;

            case SDL_EVENT_FINGER_MOTION:
                event.type = EVENT_TOUCH_MOVE;
                event.data.touch.fingerId = sdlEvent.tfinger.fingerID;
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                event.data.touch.deltaX = sdlEvent.tfinger.dx * 2.0f;
                event.data.touch.deltaY = sdlEvent.tfinger.dy * 2.0f;
                break;

            case SDL_EVENT_FINGER_CANCELED: // TODO: add cancelled native event or property for EVENT_TOUCH_END
                event.type = EVENT_TOUCH_END;
                event.data.touch.fingerId = sdlEvent.tfinger.fingerID;
                event.data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
                event.data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
                break;
            
            case SDL_EVENT_DISPLAY_ORIENTATION:
                if (!internal->capabilities.fixedDefaultOrientation) {
                    event.type = EVENT_DEVICE_ORIENTATION_CHANGED;
                    switch ((SDL_DisplayOrientation)sdlEvent.display.data1) {
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
                if (internal->capabilities.deviceType == DEVICE_DESKTOP)
                    event.type = EVENT_PLATFORM_DESKTOP_FOCUSED;
                break;
            
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                if (internal->capabilities.deviceType == DEVICE_DESKTOP)
                    event.type = EVENT_PLATFORM_DESKTOP_UNFOCUSED;
                break;
            
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
                if (internal->capabilities.deviceType == DEVICE_MOBILE)
                    event.type = EVENT_PLATFORM_MOBILE_WILL_ENTER_BACKGROUND;
                break;
            
            case SDL_EVENT_WILL_ENTER_FOREGROUND:
                if (internal->capabilities.deviceType == DEVICE_MOBILE)
                    event.type = EVENT_PLATFORM_MOBILE_WILL_ENTER_FOREGROUND;
                break;
            
            case SDL_EVENT_LOW_MEMORY:
                if (internal->capabilities.deviceType == DEVICE_MOBILE)
                    event.type = EVENT_PLATFORM_MOBILE_LOW_MEMORY;
                break;

            default:
                break;
        }

        return true;
    }

    void* Window::nwh_get() const {
        RYO_ENSURE(valid(), nullptr, "Invalid window");

        SDL_PropertiesID props = SDL_GetWindowProperties(internal->sdlWindow);
        if (props == 0) {
            return nullptr;
        }

        if (!internal->capabilities.nativeWindowHandleSupported)
            return nullptr;

        #if defined(RYO_WIN32)
        return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        #elif defined(RYO_APPLE_MAC)
        return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        #else
        #error "Native window handle sdl pointer property not specified"
        #endif
    }

    const char* Window::title_get() const {
        RYO_ENSURE(valid(), nullptr, "Invalid window");

        if (!internal->capabilities.windowTitlesSupported)
            return nullptr;

        return internal->title;
    }

    bool Window::title_set(const char* title) {
        RYO_ENSURE(valid(), false, "Invalid window");

        if (!internal->capabilities.windowTitlesSupported)
            return false;

        if (!SDL_SetWindowTitle(internal->sdlWindow, title))
            return false;

        internal->title = title;

        return true;
    }

    bool Window::size_get(unsigned int& outWidth, unsigned int& outHeight) const {
        RYO_ENSURE(valid(), false, "Invalid window");

        int width, height;
        if (!SDL_GetWindowSize(internal->sdlWindow, &width, &height))
            return false;

        outWidth = static_cast<unsigned int>(width);
        outHeight = static_cast<unsigned int>(height);

        return true;
    }

    bool Window::size_set(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(internal->capabilities.sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        return SDL_SetWindowSize(internal->sdlWindow, static_cast<int>(width), static_cast<int>(height));
    }

    bool Window::size_min_get(unsigned int& outWidth, unsigned int& outHeight) const {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(internal->capabilities.sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        int width, height;
        if (!SDL_GetWindowMinimumSize(internal->sdlWindow, &width, &height))
            return false;

        outWidth = static_cast<unsigned int>(width);
        outHeight = static_cast<unsigned int>(height);

        return true;
    }

    bool Window::size_min_set(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(internal->capabilities.sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        return SDL_SetWindowMinimumSize(internal->sdlWindow, static_cast<int>(width), static_cast<int>(height));
    }

    bool Window::size_max_get(unsigned int& outWidth, unsigned int& outHeight) const {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(internal->capabilities.sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        int width, height;
        if (!SDL_GetWindowMaximumSize(internal->sdlWindow, &width, &height))
            return false;

        outWidth = static_cast<unsigned int>(width);
        outHeight = static_cast<unsigned int>(height);

        return true;
    }

    bool Window::size_max_set(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(internal->capabilities.sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        return SDL_SetWindowMaximumSize(internal->sdlWindow, static_cast<int>(width), static_cast<int>(height));
    }

    float Window::aspect_ratio_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid window");

        unsigned int width, height;
        if (!size_get(width, height))
            return 0.0f;

        return static_cast<float>(width) / static_cast<float>(height);
    }

    float Window::hidpi_scale_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid window");

        float scale = SDL_GetWindowDisplayScale(internal->sdlWindow);

        return scale;
    }

    SafeAreaInsets Window::safe_area_insets_get() const {
        RYO_ENSURE(valid(), {}, "Invalid window");

        SDL_Rect safeArea;
        if (!SDL_GetWindowSafeArea(internal->sdlWindow, &safeArea))
            return {};

        unsigned int width, height;
        if (!size_get(width, height))
            return {};

        float insetTop = static_cast<float>(safeArea.y);
        float insetBottom = static_cast<float>(height - (safeArea.y + safeArea.h));
        float insetLeft = static_cast<float>(safeArea.x);
        float insetRight = static_cast<float>(width - (safeArea.x + safeArea.w));

        SafeAreaInsets insets;
        insets.top = insetTop;
        insets.bottom = insetBottom;
        insets.left = insetLeft;
        insets.right = insetRight;

        return insets;
    }

    unsigned int Window::flags_get() const {
        RYO_ENSURE(valid(), WINDOW_FLAG_NONE, "Invalid window");
        RYO_ENSURE(internal->capabilities.deviceType == DEVICE_DESKTOP, WINDOW_FLAG_NONE, "Window flags aren't supported for this platform");

        return internal->flags;
    }

    bool Window::flags_set(unsigned int flags) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(internal->capabilities.deviceType == DEVICE_DESKTOP, false, "Window flags aren't supported for this platform");

        bool fail = false;

        unsigned int added = flags | ~internal->flags;
        unsigned int removed = internal->flags | ~flags;

        if ((added & WINDOW_FLAG_RESIZABLE) == WINDOW_FLAG_RESIZABLE) {
            if (!SDL_SetWindowResizable(internal->sdlWindow, true))
                fail = true;
        } else if ((removed & WINDOW_FLAG_RESIZABLE) == WINDOW_FLAG_RESIZABLE) {
            if (!SDL_SetWindowResizable(internal->sdlWindow, false))
                fail = true;
        }

        if ((added & WINDOW_FLAG_FULLSCREEN) == WINDOW_FLAG_FULLSCREEN) {
            if (!SDL_SetWindowFullscreen(internal->sdlWindow, true))
                fail = true;
        } else if ((removed & WINDOW_FLAG_FULLSCREEN) == WINDOW_FLAG_FULLSCREEN) {
            if (!SDL_SetWindowFullscreen(internal->sdlWindow, false))
                fail = true;
        }

        if ((added & WINDOW_FLAG_BORDERLESS) == WINDOW_FLAG_BORDERLESS) {
            if (!SDL_SetWindowBordered(internal->sdlWindow, false))
                fail = true;
        } else if ((removed & WINDOW_FLAG_BORDERLESS) == WINDOW_FLAG_BORDERLESS) {
            if (!SDL_SetWindowBordered(internal->sdlWindow, true))
                fail = true;
        }

        if ((added & WINDOW_FLAG_HIDPI) == WINDOW_FLAG_HIDPI ||
            ((removed & WINDOW_FLAG_HIDPI) == WINDOW_FLAG_HIDPI))
            fail = true;
        
        return !fail;
    }

    unsigned int Window::orientations_get() const {
        RYO_ENSURE(valid(), ORIENTATION_NONE, "Invalid window");

        return internal->capabilities.fixedDefaultOrientation ? ORIENTATION_NONE : internal->orientations;
    }

    bool Window::orientations_set(unsigned int orientations) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(internal->capabilities.fixedDefaultOrientation == false, false, "Platform doesn't support orientations other than default");

        char hintString[256] = "";

        if ((orientations & ORIENTATION_LANDSCAPE) == ORIENTATION_LANDSCAPE)
            strcat(hintString, "LandscapeLeft ");

        if ((orientations & ORIENTATION_LANDSCAPE_FLIPPED) == ORIENTATION_LANDSCAPE_FLIPPED)
            strcat(hintString, "LandscapeRight ");

        if ((orientations & ORIENTATION_PORTRAIT) == ORIENTATION_PORTRAIT)
            strcat(hintString, "Portrait ");

        if ((orientations & ORIENTATION_PORTRAIT_FLIPPED) == ORIENTATION_PORTRAIT_FLIPPED)
            strcat(hintString, "PortraitUpsideDown");

        size_t len = strlen(hintString);
        // if (len < 2) {
        //     hintString = "LandscapeLeft ";
        //     len = strlen(hintString);
        // }

        if (len > 0)
            hintString[len - 1] = '\0';

        if (!SDL_SetHint(SDL_HINT_ORIENTATIONS, hintString))
            return false;

        internal->orientations = orientations;
    }

    unsigned int Window::orientation_get() const {
        RYO_ENSURE(valid(), ORIENTATION_NONE, "Invalid window");
        
        if (internal->capabilities.fixedDefaultOrientation)
            return ORIENTATION_NONE;

        SDL_DisplayID displayId = SDL_GetDisplayForWindow(internal->sdlWindow);
        if (displayId == 0)
            return ORIENTATION_NONE;

        SDL_DisplayOrientation orientation = SDL_GetCurrentDisplayOrientation(displayId);

        switch (orientation) {
            case SDL_ORIENTATION_LANDSCAPE:
                return ORIENTATION_LANDSCAPE;
            
            case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
                return ORIENTATION_LANDSCAPE_FLIPPED;

            case SDL_ORIENTATION_PORTRAIT:
                return ORIENTATION_PORTRAIT;
            
            case SDL_ORIENTATION_PORTRAIT_FLIPPED:
                return ORIENTATION_PORTRAIT_FLIPPED;
            
            default:
                return ORIENTATION_NONE;
        }
    }

    const PlatformCapabilities* Window::capabilities_get() const {
        return internal != nullptr ? &internal->capabilities : nullptr;
    }
}