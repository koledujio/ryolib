#import "../private.h"
#include <SDL3/SDL_error.h>
#include <stdlib.h>
#include <string.h>

@implementation RYWindow

@synthesize application = _application;
@synthesize windowId = _windowId;
@synthesize title = _title;
@synthesize flags = _flags;
@synthesize allowedOrientations = _allowedOrientations;
@synthesize internalWindow = _internal;

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                                   title:(RYString *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height {
    return [self newWindowWithApplication:application
                                    title:title
                                    width:width
                                   height:height
                                    flags:RYWindowFlagNone
                      allowedOrientations:RYDisplayOrientationLandscape
                                    error:NULL];
}

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                               titleUTF8:(const char *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height {
    return [self newWindowWithApplication:application
                                titleUTF8:title
                                    width:width
                                   height:height
                                    flags:RYWindowFlagNone
                      allowedOrientations:RYDisplayOrientationLandscape
                                    error:NULL];
}

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                                   title:(RYString *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height
                                   flags:(unsigned int)flags
                     allowedOrientations:(unsigned int)allowedOrientations
                                   error:(RYError **)error {
    return [[self alloc] initWithApplication:application
                                       title:title
                                       width:width
                                      height:height
                                       flags:flags
                         allowedOrientations:allowedOrientations
                                       error:error];
}

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                               titleUTF8:(const char *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height
                                   flags:(unsigned int)flags
                     allowedOrientations:(unsigned int)allowedOrientations
                                   error:(RYError **)error {
    return [[self alloc] initWithApplication:application
                                   titleUTF8:title
                                       width:width
                                      height:height
                                       flags:flags
                         allowedOrientations:allowedOrientations
                                       error:error];
}

- (instancetype)initWithApplication:(RYApplication *)application
                              title:(RYString *)title
                              width:(unsigned int)width
                             height:(unsigned int)height
                              flags:(unsigned int)flags
                allowedOrientations:(unsigned int)allowedOrientations
                              error:(RYError **)error {
    return [self initWithApplication:application
                           titleUTF8:title ? title.UTF8String : NULL
                               width:width
                              height:height
                               flags:flags
                 allowedOrientations:allowedOrientations
                               error:error];
}

- (instancetype)initWithApplication:(RYApplication *)application
                          titleUTF8:(const char *)title
                              width:(unsigned int)width
                             height:(unsigned int)height
                              flags:(unsigned int)flags
                allowedOrientations:(unsigned int)allowedOrientations
                              error:(RYError **)error {
    self = [super init];
    if (!self) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInitFailed message:"Failed to allocate RYWindow"];
        }
        return nil;
    }

    if (!application || !application.internalApplication) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidApplication message:"Invalid application instance provided for window creation"];
        }
        [self release];
        return nil;
    }

    if (!title) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument message:"Null title string provided for window creation"];
        }
        [self release];
        return nil;
    }

    const RYPlatformCapabilities *capabilities = ry_platform_get_capabilities();

    uint64_t sdlFlags = 0;
    if (capabilities->deviceType == RYDeviceTypeDesktop || capabilities->platformType == RYPlatformTypeIOS)
        sdlFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (capabilities->deviceType == RYDeviceTypeDesktop) {
        if ((flags & RYWindowFlagResizable) == RYWindowFlagResizable)
            sdlFlags |= SDL_WINDOW_RESIZABLE;
        if ((flags & RYWindowFlagFullscreen) == RYWindowFlagFullscreen)
            sdlFlags |= SDL_WINDOW_FULLSCREEN;
        if ((flags & RYWindowFlagBorderless) == RYWindowFlagBorderless)
            sdlFlags |= SDL_WINDOW_BORDERLESS;
    }

    unsigned int newWidth = width;
    unsigned int newHeight = height;
    ry_platform_get_forced_resolution(&newWidth, &newHeight);

    SDL_Window *sdlWindow = SDL_CreateWindow(title, (int)newWidth, (int)newHeight, sdlFlags);
    if (!sdlWindow) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingWindowCreationFailed message:SDL_GetError()];
        }
        [self release];
        return nil;
    }

    SDL_WindowID windowID = SDL_GetWindowID(sdlWindow);
    if (windowID == 0) {
        SDL_DestroyWindow(sdlWindow);
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingWindowCreationFailed message:SDL_GetError()];
        }
        [self release];
        return nil;
    }

    RYInternalWindow *intern = (RYInternalWindow *)malloc(sizeof(RYInternalWindow));
    if (!intern) {
        SDL_DestroyWindow(sdlWindow);
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralOutOfMemory message:"Failed to allocate internal window memory"];
        }
        [self release];
        return nil;
    }

    _application = [application retain];
    intern->sdlWindow = sdlWindow;
    intern->title = capabilities->windowTitlesSupported ? [[RYString alloc] initWithUTF8String:title] : nil;
    intern->flags = (capabilities->deviceType == RYDeviceTypeDesktop ||
                     (flags & RYWindowFlagStrictOrientations) == RYWindowFlagStrictOrientations)
                        ? flags
                        : RYWindowFlagNone;
    intern->orientations = capabilities->fixedDefaultOrientation ? RYDisplayOrientationNone : allowedOrientations;
    intern->id = (RYWindowID)windowID;
    intern->metalView = NULL;

    _internal = intern;
    _windowId = intern->id;
    _flags = intern->flags;
    _allowedOrientations = intern->orientations;
    _title = intern->title ? [intern->title retain] : nil;

    if (!capabilities->fixedDefaultOrientation) {
        [self setAllowedOrientations:allowedOrientations error:NULL];
    }

    return self;
}

- (void)dealloc {
    if (_internal) {
        RYInternalWindow *intern = (RYInternalWindow *)_internal;
        if (intern->metalView) {
            SDL_Metal_DestroyView(intern->metalView);
            intern->metalView = NULL;
        }
        if (intern->sdlWindow) {
            SDL_DestroyWindow(intern->sdlWindow);
            intern->sdlWindow = NULL;
        }
        if (intern->title) {
            [intern->title release];
            intern->title = nil;
        }
        free(intern);
        _internal = NULL;
    }

    [_title release];
    _title = nil;
    [_application release];
    _application = nil;

    [super dealloc];
}

- (void *)nativeWindowHandle {
    if (!_internal) return NULL;

    const RYPlatformCapabilities *capabilities = ry_platform_get_capabilities();
    if (!capabilities->nativeWindowHandleSupported)
        return NULL;

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    SDL_PropertiesID props = SDL_GetWindowProperties(intern->sdlWindow);
    if (props == 0) return NULL;

#if defined(RYO_WIN32)
    return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#elif defined(RYO_APPLE_MAC)
    return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
#elif defined(RYO_VITA)
    return NULL;
#elif defined(RYO_APPLE_IOS)
    if (!intern->metalView)
        intern->metalView = SDL_Metal_CreateView(intern->sdlWindow);
    return intern->metalView;
#else
    return NULL;
#endif
}

- (const char *)titleUTF8 {
    if (!_internal) return NULL;
    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    return intern->title ? intern->title.UTF8String : NULL;
}

- (BOOL)setTitleUTF8:(const char *)title {
    return [self setTitleUTF8:title error:NULL];
}

- (BOOL)setTitleUTF8:(const char *)title error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidWindow message:"Invalid window"];
        return NO;
    }

    const RYPlatformCapabilities *capabilities = ry_platform_get_capabilities();
    if (!capabilities->windowTitlesSupported) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingFeatureNotSupported message:"Window titles not supported on this platform"];
        return NO;
    }

    if (!title) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument message:"Null title string provided"];
        return NO;
    }

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    if (!SDL_SetWindowTitle(intern->sdlWindow, title)) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGeneralError message:SDL_GetError()];
        return NO;
    }

    if (intern->title) [intern->title release];
    intern->title = [[RYString alloc] initWithUTF8String:title];

    [_title release];
    _title = [intern->title retain];

    return YES;
}

- (void)setTitle:(RYString *)title {
    [self setTitleUTF8:title ? title.UTF8String : NULL error:NULL];
}

- (RYWindowSize)size {
    unsigned int width = 0, height = 0;
    [self getSize:&width height:&height];
    return RYWindowSizeMake(width, height);
}

- (void)setSize:(RYWindowSize)size {
    [self setSizeWithWidth:size.width height:size.height error:NULL];
}

- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight {
    if (!_internal) return NO;
    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    int w = 0, h = 0;
    if (!SDL_GetWindowSize(intern->sdlWindow, &w, &h)) return NO;
    if (outWidth) *outWidth = (unsigned int)w;
    if (outHeight) *outHeight = (unsigned int)h;
    return YES;
}

- (BOOL)setSizeWithWidth:(unsigned int)width height:(unsigned int)height {
    return [self setSizeWithWidth:width height:height error:NULL];
}

- (BOOL)setSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidWindow message:"Invalid window"];
        return NO;
    }

    if (ry_platform_get_capabilities()->sizeManagementType != RYSizeManagementTypeFreeResizable) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingFeatureNotSupported message:"Platform does not support resizing windows"];
        return NO;
    }

    if (width == 0 || height == 0) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidSize message:"Window width and height must be greater than zero"];
        return NO;
    }

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    if (!SDL_SetWindowSize(intern->sdlWindow, (int)width, (int)height)) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGeneralError message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (RYWindowSize)minSize {
    unsigned int width = 0, height = 0;
    [self getMinSize:&width height:&height];
    return RYWindowSizeMake(width, height);
}

- (void)setMinSize:(RYWindowSize)minSize {
    [self setMinSizeWithWidth:minSize.width height:minSize.height error:NULL];
}

- (BOOL)getMinSize:(unsigned int *)outWidth height:(unsigned int *)outHeight {
    if (!_internal) return NO;
    if (ry_platform_get_capabilities()->sizeManagementType != RYSizeManagementTypeFreeResizable) return NO;
    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    int w = 0, h = 0;
    if (!SDL_GetWindowMinimumSize(intern->sdlWindow, &w, &h)) return NO;
    if (outWidth) *outWidth = (unsigned int)w;
    if (outHeight) *outHeight = (unsigned int)h;
    return YES;
}

- (BOOL)setMinSizeWithWidth:(unsigned int)width height:(unsigned int)height {
    return [self setMinSizeWithWidth:width height:height error:NULL];
}

- (BOOL)setMinSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidWindow message:"Invalid window"];
        return NO;
    }

    if (ry_platform_get_capabilities()->sizeManagementType != RYSizeManagementTypeFreeResizable) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingFeatureNotSupported message:"Platform does not support minimum window dimensions"];
        return NO;
    }

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    if (!SDL_SetWindowMinimumSize(intern->sdlWindow, (int)width, (int)height)) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGeneralError message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (RYWindowSize)maxSize {
    unsigned int width = 0, height = 0;
    [self getMaxSize:&width height:&height];
    return RYWindowSizeMake(width, height);
}

- (void)setMaxSize:(RYWindowSize)maxSize {
    [self setMaxSizeWithWidth:maxSize.width height:maxSize.height error:NULL];
}

- (BOOL)getMaxSize:(unsigned int *)outWidth height:(unsigned int *)outHeight {
    if (!_internal) return NO;
    if (ry_platform_get_capabilities()->sizeManagementType != RYSizeManagementTypeFreeResizable) return NO;
    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    int w = 0, h = 0;
    if (!SDL_GetWindowMaximumSize(intern->sdlWindow, &w, &h)) return NO;
    if (outWidth) *outWidth = (unsigned int)w;
    if (outHeight) *outHeight = (unsigned int)h;
    return YES;
}

- (BOOL)setMaxSizeWithWidth:(unsigned int)width height:(unsigned int)height {
    return [self setMaxSizeWithWidth:width height:height error:NULL];
}

- (BOOL)setMaxSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidWindow message:"Invalid window"];
        return NO;
    }

    if (ry_platform_get_capabilities()->sizeManagementType != RYSizeManagementTypeFreeResizable) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingFeatureNotSupported message:"Platform does not support maximum window dimensions"];
        return NO;
    }

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    if (!SDL_SetWindowMaximumSize(intern->sdlWindow, (int)width, (int)height)) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGeneralError message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (float)aspectRatio {
    unsigned int width = 0, height = 0;
    if (![self getSize:&width height:&height] || height == 0) return 0.0f;
    return (float)width / (float)height;
}

- (float)hidpiScale {
    if (!_internal) return 0.0f;
    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    return ry_platform_get_content_scale(intern->sdlWindow);
}

- (RYSafeAreaInsets)safeAreaInsets {
    if (!_internal) return RYSafeAreaInsetsMake(0.0f, 0.0f, 0.0f, 0.0f);
    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    SDL_Rect safeArea;
    if (!SDL_GetWindowSafeArea(intern->sdlWindow, &safeArea)) {
        return RYSafeAreaInsetsMake(0.0f, 0.0f, 0.0f, 0.0f);
    }

    unsigned int width = 0, height = 0;
    if (![self getSize:&width height:&height]) {
        return RYSafeAreaInsetsMake(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float insetTop = (float)safeArea.y;
    float insetBottom = (float)(height - (safeArea.y + safeArea.h));
    float insetLeft = (float)safeArea.x;
    float insetRight = (float)(width - (safeArea.x + safeArea.w));

    return RYSafeAreaInsetsMake(insetTop, insetBottom, insetLeft, insetRight);
}

- (void)setFlags:(unsigned int)flags {
    [self setFlags:flags error:NULL];
}

- (BOOL)setFlags:(unsigned int)flags error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidWindow message:"Invalid window"];
        return NO;
    }

    const RYPlatformCapabilities *capabilities = ry_platform_get_capabilities();
    if (capabilities->deviceType != RYDeviceTypeDesktop) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingFeatureNotSupported message:"Window flags are not supported for this platform"];
        return NO;
    }

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    BOOL fail = NO;
    unsigned int added = flags & ~intern->flags;
    unsigned int removed = intern->flags & ~flags;

    if ((added & RYWindowFlagResizable) == RYWindowFlagResizable) {
        if (!SDL_SetWindowResizable(intern->sdlWindow, true)) fail = YES;
    } else if ((removed & RYWindowFlagResizable) == RYWindowFlagResizable) {
        if (!SDL_SetWindowResizable(intern->sdlWindow, false)) fail = YES;
    }

    if ((added & RYWindowFlagFullscreen) == RYWindowFlagFullscreen) {
        if (!SDL_SetWindowFullscreen(intern->sdlWindow, true)) fail = YES;
    } else if ((removed & RYWindowFlagFullscreen) == RYWindowFlagFullscreen) {
        if (!SDL_SetWindowFullscreen(intern->sdlWindow, false)) fail = YES;
    }

    if ((added & RYWindowFlagBorderless) == RYWindowFlagBorderless) {
        if (!SDL_SetWindowBordered(intern->sdlWindow, false)) fail = YES;
    } else if ((removed & RYWindowFlagBorderless) == RYWindowFlagBorderless) {
        if (!SDL_SetWindowBordered(intern->sdlWindow, true)) fail = YES;
    }

    intern->flags = flags;
    _flags = flags;

    return !fail;
}

- (void)setAllowedOrientations:(unsigned int)allowedOrientations {
    [self setAllowedOrientations:allowedOrientations error:NULL];
}

- (BOOL)setAllowedOrientations:(unsigned int)orientations error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidWindow message:"Invalid window"];
        return NO;
    }

    const RYPlatformCapabilities *capabilities = ry_platform_get_capabilities();
    if (capabilities->fixedDefaultOrientation) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingFeatureNotSupported message:"Platform does not support orientations other than default"];
        return NO;
    }

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    unsigned int effectiveOrientations = orientations;

    if (ry_platform_is_tablet() && (intern->flags & RYWindowFlagStrictOrientations) != RYWindowFlagStrictOrientations) {
        effectiveOrientations = RYDisplayOrientationAll;
    }

    char hintString[256] = "";
    if ((effectiveOrientations & RYDisplayOrientationLandscape) == RYDisplayOrientationLandscape)
        strcat(hintString, "LandscapeLeft ");
    if ((effectiveOrientations & RYDisplayOrientationLandscapeFlipped) == RYDisplayOrientationLandscapeFlipped)
        strcat(hintString, "LandscapeRight ");
    if ((effectiveOrientations & RYDisplayOrientationPortrait) == RYDisplayOrientationPortrait)
        strcat(hintString, "Portrait ");
    if ((effectiveOrientations & RYDisplayOrientationPortraitFlipped) == RYDisplayOrientationPortraitFlipped)
        strcat(hintString, "PortraitUpsideDown");

    size_t len = strlen(hintString);
    if (len > 0) hintString[len - 1] = '\0';

    if (!SDL_SetHint(SDL_HINT_ORIENTATIONS, hintString)) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGeneralError message:SDL_GetError()];
        return NO;
    }

    intern->orientations = orientations;
    _allowedOrientations = orientations;

    return YES;
}

- (unsigned int)orientation {
    if (!_internal) return RYDisplayOrientationNone;
    if (ry_platform_get_capabilities()->fixedDefaultOrientation) return RYDisplayOrientationNone;

    RYInternalWindow *intern = (RYInternalWindow *)_internal;
    SDL_DisplayID displayId = SDL_GetDisplayForWindow(intern->sdlWindow);
    if (displayId == 0) return RYDisplayOrientationNone;

    SDL_DisplayOrientation orientation = SDL_GetCurrentDisplayOrientation(displayId);
    switch (orientation) {
        case SDL_ORIENTATION_LANDSCAPE:
            return RYDisplayOrientationLandscape;
        case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
            return RYDisplayOrientationLandscapeFlipped;
        case SDL_ORIENTATION_PORTRAIT:
            return RYDisplayOrientationPortrait;
        case SDL_ORIENTATION_PORTRAIT_FLIPPED:
            return RYDisplayOrientationPortraitFlipped;
        default:
            return RYDisplayOrientationNone;
    }
}

@end
