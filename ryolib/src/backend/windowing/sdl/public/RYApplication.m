#import "../private.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <stdlib.h>

@implementation RYApplication

@synthesize capabilities = _capabilities;
@synthesize internalApplication = _internal;

+ (instancetype)newApplication {
    return [[self alloc] initWithError:NULL];
}

+ (instancetype)newApplicationWithError:(RYError **)error {
    return [[self alloc] initWithError:error];
}

- (instancetype)init {
    return [self initWithError:NULL];
}

- (instancetype)initWithError:(RYError **)error {
    self = [super init];
    if (!self) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInitFailed message:"Failed to allocate RYApplication"];
        }
        return nil;
    }

    if (!ry_sdl_initialized) {
        if (!ry_sdl_initialize()) {
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInitializationFailed message:SDL_GetError()];
            }
            [self release];
            return nil;
        }
    }

    RYInternalApplication *intern = (RYInternalApplication *)malloc(sizeof(RYInternalApplication));
    if (!intern) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralOutOfMemory message:"Failed to allocate RYApplication internal memory"];
        }
        [self release];
        return nil;
    }
    intern->dummy = 0;
    _internal = intern;
    _capabilities = *ry_platform_get_capabilities();

    return self;
}

- (void)dealloc {
    if (_internal) {
        free(_internal);
        _internal = NULL;
        ry_sdl_quit();
    }
    [super dealloc];
}

- (BOOL)pollEvent:(RYEvent *)event {
    if (!_internal || !event) {
        return NO;
    }

    SDL_Event sdlEvent;
    if (!SDL_PollEvent(&sdlEvent)) {
        return NO;
    }

    event->type = RYEventTypeNone;
    event->windowId = 0;

    const RYPlatformCapabilities *capabilities = &_capabilities;

    switch (sdlEvent.type) {
        case SDL_EVENT_QUIT:
            event->type = RYEventTypeQuit;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            event->type = RYEventTypeWindowResized;
            event->windowId = (RYWindowID)sdlEvent.window.windowID;
            event->data.window.width = (unsigned int)sdlEvent.window.data1;
            event->data.window.height = (unsigned int)sdlEvent.window.data2;
            break;

        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            event->type = RYEventTypeWindowHiDPIScaleChanged;
            event->windowId = (RYWindowID)sdlEvent.window.windowID;
            break;

        case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
            event->type = RYEventTypeWindowSafeAreaChanged;
            event->windowId = (RYWindowID)sdlEvent.window.windowID;
            break;

        case SDL_EVENT_GAMEPAD_ADDED:
            event->type = RYEventTypePadAdded;
            event->data.pad.id = sdlEvent.gdevice.which;
            break;

        case SDL_EVENT_GAMEPAD_REMOVED:
            event->type = RYEventTypePadRemoved;
            event->data.pad.id = sdlEvent.gdevice.which;
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            event->type = RYEventTypePadButtonDown;
            event->data.pad.id = sdlEvent.gbutton.which;
            event->data.pad.button = ry_gamepad_get_ryo_button(sdlEvent.gbutton.button);
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            event->type = RYEventTypePadButtonUp;
            event->data.pad.id = sdlEvent.gbutton.which;
            event->data.pad.button = ry_gamepad_get_ryo_button(sdlEvent.gbutton.button);
            break;

        case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
            event->type = RYEventTypePadAxisMotion;
            event->data.pad.id = sdlEvent.gaxis.which;
            event->data.pad.axis = ry_gamepad_get_ryo_axis(sdlEvent.gaxis.axis);
            float val = sdlEvent.gaxis.value / 32767.0f;
            if (val < -1.0f) val = -1.0f;
            if (val > 1.0f) val = 1.0f;
            event->data.pad.axisValue = val;
            break;
        }

        case SDL_EVENT_KEY_DOWN:
            event->type = RYEventTypeKeyDown;
            event->windowId = (RYWindowID)sdlEvent.key.windowID;
            event->data.key.code = ry_keyboard_get_ryo_key(sdlEvent.key.scancode);
            event->data.key.isRepeat = sdlEvent.key.repeat ? YES : NO;
            break;

        case SDL_EVENT_KEY_UP:
            event->type = RYEventTypeKeyUp;
            event->windowId = (RYWindowID)sdlEvent.key.windowID;
            event->data.key.code = ry_keyboard_get_ryo_key(sdlEvent.key.scancode);
            event->data.key.isRepeat = sdlEvent.key.repeat ? YES : NO;
            break;

        case SDL_EVENT_FINGER_DOWN:
            event->type = RYEventTypeTouchBegin;
            event->windowId = (RYWindowID)sdlEvent.tfinger.windowID;
            event->data.touch.fingerId = (unsigned int)sdlEvent.tfinger.fingerID;
            event->data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
            event->data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
            break;

        case SDL_EVENT_FINGER_UP:
            event->type = RYEventTypeTouchEnd;
            event->windowId = (RYWindowID)sdlEvent.tfinger.windowID;
            event->data.touch.fingerId = (unsigned int)sdlEvent.tfinger.fingerID;
            event->data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
            event->data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
            break;

        case SDL_EVENT_FINGER_MOTION:
            event->type = RYEventTypeTouchMove;
            event->windowId = (RYWindowID)sdlEvent.tfinger.windowID;
            event->data.touch.fingerId = (unsigned int)sdlEvent.tfinger.fingerID;
            event->data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
            event->data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
            event->data.touch.deltaX = sdlEvent.tfinger.dx * 2.0f;
            event->data.touch.deltaY = sdlEvent.tfinger.dy * 2.0f;
            break;

        case SDL_EVENT_FINGER_CANCELED:
            event->type = RYEventTypeTouchEnd;
            event->windowId = (RYWindowID)sdlEvent.tfinger.windowID;
            event->data.touch.fingerId = (unsigned int)sdlEvent.tfinger.fingerID;
            event->data.touch.x = sdlEvent.tfinger.x * 2.0f - 1.0f;
            event->data.touch.y = sdlEvent.tfinger.y * 2.0f - 1.0f;
            break;

        case SDL_EVENT_DISPLAY_ORIENTATION:
            if (!capabilities->fixedDefaultOrientation) {
                event->type = RYEventTypeDeviceOrientationChanged;
                switch ((SDL_DisplayOrientation)sdlEvent.display.data1) {
                    case SDL_ORIENTATION_LANDSCAPE:
                        event->data.orientation = RYDisplayOrientationLandscape;
                        break;
                    case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
                        event->data.orientation = RYDisplayOrientationLandscapeFlipped;
                        break;
                    case SDL_ORIENTATION_PORTRAIT:
                        event->data.orientation = RYDisplayOrientationPortrait;
                        break;
                    case SDL_ORIENTATION_PORTRAIT_FLIPPED:
                        event->data.orientation = RYDisplayOrientationPortraitFlipped;
                        break;
                    default:
                        event->data.orientation = RYDisplayOrientationNone;
                        break;
                }
            }
            break;

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            if (capabilities->deviceType == RYDeviceTypeDesktop) {
                event->type = RYEventTypePlatformDesktopFocused;
                event->windowId = (RYWindowID)sdlEvent.window.windowID;
            }
            break;

        case SDL_EVENT_WINDOW_FOCUS_LOST:
            if (capabilities->deviceType == RYDeviceTypeDesktop) {
                event->type = RYEventTypePlatformDesktopUnfocused;
                event->windowId = (RYWindowID)sdlEvent.window.windowID;
            }
            break;

        case SDL_EVENT_WILL_ENTER_BACKGROUND:
            if (capabilities->deviceType == RYDeviceTypeMobile)
                event->type = RYEventTypePlatformMobileWillEnterBackground;
            break;

        case SDL_EVENT_WILL_ENTER_FOREGROUND:
            if (capabilities->deviceType == RYDeviceTypeMobile)
                event->type = RYEventTypePlatformMobileWillEnterForeground;
            break;

        case SDL_EVENT_LOW_MEMORY:
            if (capabilities->deviceType == RYDeviceTypeMobile)
                event->type = RYEventTypePlatformMobileLowMemory;
            break;

        default:
            break;
    }

    return YES;
}

@end
