#include "../private.h"

#if defined(RYO_APPLE_IOS)

#import <UIKit/UIKit.h>

namespace ryo {
    float _platform_get_content_scale(SDL_Window* sdlWindow) {
        SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
        if (props == 0) {
            return 1.0f;
        }

        UIWindow* window = (__bridge UIWindow*)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, nullptr);
        if (!window) {
            return 1.0f;
        }

        CGFloat scale = window.screen ? window.screen.nativeScale : [UIScreen mainScreen].scale;
        if (scale <= 0.0) {
            return 1.0f;
        }

        return static_cast<float>(scale);
    }

    bool _platform_is_tablet() {
        UIUserInterfaceIdiom idiom = [UIDevice currentDevice].userInterfaceIdiom;

        return idiom == UIUserInterfaceIdiomPad;
    }
}

#endif
