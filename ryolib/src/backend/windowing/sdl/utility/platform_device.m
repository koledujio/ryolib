#import "../private.h"

#if defined(RYO_APPLE_IOS)
#import <UIKit/UIKit.h>

float ry_platform_get_content_scale(SDL_Window *sdlWindow) {
    SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
    if (props == 0) {
        return 1.0f;
    }

    UIWindow *window = (__bridge UIWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, NULL);
    if (!window) {
        return 1.0f;
    }

    CGFloat scale = window.screen ? window.screen.nativeScale : [UIScreen mainScreen].scale;
    if (scale <= 0.0) {
        return 1.0f;
    }

    return (float)scale;
}

BOOL ry_platform_is_tablet(void) {
    UIUserInterfaceIdiom idiom = [UIDevice currentDevice].userInterfaceIdiom;
    return idiom == UIUserInterfaceIdiomPad;
}

#else

float ry_platform_get_content_scale(SDL_Window *sdlWindow) {
    int windowWidth = 0;
    int windowHeight = 0;
    int pixelWidth = 0;
    int pixelHeight = 0;

    if (SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight) &&
        SDL_GetWindowSizeInPixels(sdlWindow, &pixelWidth, &pixelHeight) &&
        windowWidth > 0 && windowHeight > 0) {
        return (float)pixelWidth / (float)windowWidth;
    }

    return 1.0f;
}

BOOL ry_platform_is_tablet(void) {
    return NO;
}

#endif
