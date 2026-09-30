#include "../private.h"

#include <cstdio>
#include <cstring>

#include "SDL3/SDL_error.h"

namespace ryo {
    Window::Window(
        const Application& app,
        const char* title,
        unsigned int width,
        unsigned int height,
        unsigned int flags,
        unsigned int allowedOrientations
    ) {
        RYO_ENSURE_VOID(app.valid(), "Invalid application instance provided for window creation");
        RYO_ENSURE_VOID(title != nullptr, "Null title string pointer provided");

        const PlatformCapabilities& capabilities = app.capabilities_get();

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

        unsigned int newWidth = width;
        unsigned int newHeight = height;
        _platform_get_forced_resolution(newWidth, newHeight);

        SDL_Window* window = SDL_CreateWindow(title, static_cast<int>(newWidth), static_cast<int>(newHeight), sdlFlags);

        SDL_WindowID id = SDL_GetWindowID(window);
        if (id == 0) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        if (!window) {
            error_set("Failed to create SDL window: %s", SDL_GetError());
            return;
        }

        InternalWindow* intern = new InternalWindow;
        intern->sdlWindow = window;
        intern->title = capabilities.windowTitlesSupported ? title : nullptr;
        intern->flags = capabilities.deviceType == DEVICE_DESKTOP ? flags : WINDOW_FLAG_NONE;
        intern->orientations = capabilities.fixedDefaultOrientation ? ORIENTATION_NONE : allowedOrientations;
        intern->id = static_cast<WindowID>(id);

        internal = intern;

        if (!capabilities.fixedDefaultOrientation)
            orientations_set(allowedOrientations);
    }

    Window::~Window() {
        if (!internal)
            return;

        if (internal->sdlWindow)
            SDL_DestroyWindow(internal->sdlWindow);

        delete internal;
        internal = nullptr;
    }

    Window::Window(Window&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Window& Window::operator=(Window&& other) {
        if (this == &other)
            return *this;

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

    WindowID Window::id_get() const {
        RYO_ENSURE(valid(), 0, "Invalid window");

        return internal->id;
    }

    void* Window::nwh_get() const {
        RYO_ENSURE(valid(), nullptr, "Invalid window");

        const PlatformCapabilities& capabilities = _platform_get_capabilities();
        if (!capabilities.nativeWindowHandleSupported)
            return nullptr;

        SDL_PropertiesID props = SDL_GetWindowProperties(internal->sdlWindow);
        if (props == 0) {
            return nullptr;
        }

        #if defined(RYO_WIN32)
        return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        #elif defined(RYO_APPLE_MAC)
        return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        #elif defined(RYO_VITA)
        return nullptr;
        #else
        #pragma error "Native window handle sdl pointer property not specified"
        #endif
    }

    const char* Window::title_get() const {
        RYO_ENSURE(valid(), nullptr, "Invalid window");

        const PlatformCapabilities& capabilities = _platform_get_capabilities();
        if (!capabilities.windowTitlesSupported)
            return nullptr;

        return internal->title;
    }

    bool Window::title_set(const char* title) {
        RYO_ENSURE(valid(), false, "Invalid window");

        const PlatformCapabilities& capabilities = _platform_get_capabilities();
        if (!capabilities.windowTitlesSupported)
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
        RYO_ENSURE(_platform_get_capabilities().sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        return SDL_SetWindowSize(internal->sdlWindow, static_cast<int>(width), static_cast<int>(height));
    }

    bool Window::size_min_get(unsigned int& outWidth, unsigned int& outHeight) const {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(_platform_get_capabilities().sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        int width, height;
        if (!SDL_GetWindowMinimumSize(internal->sdlWindow, &width, &height))
            return false;

        outWidth = static_cast<unsigned int>(width);
        outHeight = static_cast<unsigned int>(height);

        return true;
    }

    bool Window::size_min_set(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(_platform_get_capabilities().sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        return SDL_SetWindowMinimumSize(internal->sdlWindow, static_cast<int>(width), static_cast<int>(height));
    }

    bool Window::size_max_get(unsigned int& outWidth, unsigned int& outHeight) const {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(_platform_get_capabilities().sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

        int width, height;
        if (!SDL_GetWindowMaximumSize(internal->sdlWindow, &width, &height))
            return false;

        outWidth = static_cast<unsigned int>(width);
        outHeight = static_cast<unsigned int>(height);

        return true;
    }

    bool Window::size_max_set(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(_platform_get_capabilities().sizeManagementType == SIZE_FREE_RESIZABLE, false, "Platform doesn't support resizing windows");

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
        RYO_ENSURE(_platform_get_capabilities().deviceType == DEVICE_DESKTOP, WINDOW_FLAG_NONE, "Window flags aren't supported for this platform");

        return internal->flags;
    }

    bool Window::flags_set(unsigned int flags) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(_platform_get_capabilities().deviceType == DEVICE_DESKTOP, false, "Window flags aren't supported for this platform");

        bool fail = false;

        unsigned int added = flags & ~internal->flags;
        unsigned int removed = internal->flags & ~flags;

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

        return _platform_get_capabilities().fixedDefaultOrientation ? ORIENTATION_NONE : internal->orientations;
    }

    bool Window::orientations_set(unsigned int orientations) {
        RYO_ENSURE(valid(), false, "Invalid window");
        RYO_ENSURE(_platform_get_capabilities().fixedDefaultOrientation == false, false, "Platform doesn't support orientations other than default");

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
        if (len > 0)
            hintString[len - 1] = '\0';

        if (!SDL_SetHint(SDL_HINT_ORIENTATIONS, hintString))
            return false;

        internal->orientations = orientations;

        return true;
    }

    unsigned int Window::orientation_get() const {
        RYO_ENSURE(valid(), ORIENTATION_NONE, "Invalid window");
        
        if (_platform_get_capabilities().fixedDefaultOrientation)
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
}