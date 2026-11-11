#import "../private.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gamepad.h>
#include <stdlib.h>

@implementation RYGamepad

@synthesize application = _application;
@synthesize gamepadId = _gamepadId;
@synthesize type = _type;
@synthesize name = _name;
@synthesize internalGamepad = _internal;

+ (instancetype)newGamepadWithApplication:(RYApplication *)application id:(int)gamepadId {
    return [self newGamepadWithApplication:application id:gamepadId error:NULL];
}

+ (instancetype)newGamepadWithApplication:(RYApplication *)application id:(int)gamepadId error:(RYError **)error {
    return [[self alloc] initWithApplication:application id:gamepadId error:error];
}

- (instancetype)initWithApplication:(RYApplication *)application id:(int)gamepadId error:(RYError **)error {
    self = [super init];
    if (!self) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInitFailed message:"Failed to allocate RYGamepad"];
        }
        return nil;
    }

    if (!application || !application.internalApplication) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidApplication message:"Invalid application instance provided for gamepad connection"];
        }
        [self release];
        return nil;
    }

    SDL_Gamepad *sdlGamepad = SDL_OpenGamepad((SDL_JoystickID)gamepadId);
    if (!sdlGamepad) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGamepadCreationFailed message:SDL_GetError()];
        }
        [self release];
        return nil;
    }

    SDL_GamepadType sdlType = SDL_GetGamepadType(sdlGamepad);

    RYInternalGamepad *intern = (RYInternalGamepad *)malloc(sizeof(RYInternalGamepad));
    if (!intern) {
        SDL_CloseGamepad(sdlGamepad);
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralOutOfMemory message:"Failed to allocate internal gamepad memory"];
        }
        [self release];
        return nil;
    }

    _application = [application retain];
    intern->sdlGamepad = sdlGamepad;
    intern->id = gamepadId;
    intern->type = ry_gamepad_get_ryo_type(sdlType);

    _internal = intern;
    _gamepadId = gamepadId;
    _type = intern->type;

    const char *nameStr = SDL_GetGamepadName(sdlGamepad);
    _name = [[RYString alloc] initWithUTF8String:nameStr ? nameStr : "Unknown Gamepad"];

    return self;
}

- (void)dealloc {
    if (_internal) {
        RYInternalGamepad *intern = (RYInternalGamepad *)_internal;
        if (intern->sdlGamepad) {
            SDL_CloseGamepad(intern->sdlGamepad);
            intern->sdlGamepad = NULL;
        }
        free(intern);
        _internal = NULL;
    }

    [_name release];
    _name = nil;
    [_application release];
    _application = nil;

    [super dealloc];
}

- (const char *)nameUTF8 {
    return _name ? _name.UTF8String : "Unknown Gamepad";
}

- (BOOL)setRumbleWithLowFrequency:(uint16_t)lowFreq
                    highFrequency:(uint16_t)highFreq
                         duration:(uint32_t)durationMs {
    return [self setRumbleWithLowFrequency:lowFreq highFrequency:highFreq duration:durationMs error:NULL];
}

- (BOOL)setRumbleWithLowFrequency:(uint16_t)lowFreq
                    highFrequency:(uint16_t)highFreq
                         duration:(uint32_t)durationMs
                            error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidGamepad message:"Invalid gamepad"];
        return NO;
    }

    RYInternalGamepad *intern = (RYInternalGamepad *)_internal;
    if (!SDL_RumbleGamepad(intern->sdlGamepad, lowFreq, highFreq, durationMs)) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGeneralError message:SDL_GetError()];
        return NO;
    }

    return YES;
}

- (BOOL)setTriggerRumbleWithLeftTrigger:(uint16_t)leftTrigger
                          rightTrigger:(uint16_t)rightTrigger
                              duration:(uint32_t)durationMs {
    return [self setTriggerRumbleWithLeftTrigger:leftTrigger rightTrigger:rightTrigger duration:durationMs error:NULL];
}

- (BOOL)setTriggerRumbleWithLeftTrigger:(uint16_t)leftTrigger
                          rightTrigger:(uint16_t)rightTrigger
                              duration:(uint32_t)durationMs
                                 error:(RYError **)error {
    if (!_internal) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingInvalidGamepad message:"Invalid gamepad"];
        return NO;
    }

    RYInternalGamepad *intern = (RYInternalGamepad *)_internal;
    if (!SDL_RumbleGamepadTriggers(intern->sdlGamepad, leftTrigger, rightTrigger, durationMs)) {
        if (error) *error = [[RYError alloc] initWithCode:RYErrorCodeWindowingGeneralError message:SDL_GetError()];
        return NO;
    }

    return YES;
}

@end
