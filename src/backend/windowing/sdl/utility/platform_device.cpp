#include "../private.h"

namespace ryo {
#if !defined(RYO_APPLE_IOS)
    float _platform_get_content_scale(SDL_Window* sdlWindow) {
        int windowWidth = 0;
        int windowHeight = 0;
        int pixelWidth = 0;
        int pixelHeight = 0;

        if (SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight) &&
            SDL_GetWindowSizeInPixels(sdlWindow, &pixelWidth, &pixelHeight) &&
            windowWidth > 0 && windowHeight > 0) {
            return static_cast<float>(pixelWidth) / static_cast<float>(windowWidth);
        }

        return 1.0f;
    }

    bool _platform_is_tablet() {
        return false;
    }
#endif
}
