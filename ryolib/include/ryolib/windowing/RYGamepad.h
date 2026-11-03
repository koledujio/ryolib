#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYString.h"
#import "../objc/RYError.h"
#import "RYWindowingTypes.h"
#import "RYApplication.h"

/**
 * @brief Represents a connected gamepad controller.
 */
@interface RYGamepad : RYObject {
@protected
    RYApplication *_application;
    int _gamepadId;
    RYGamepadType _type;
    RYString *_name;
    void *_internal;
}

@property (nonatomic, readonly) RYApplication *application;
@property (nonatomic, readonly) BOOL isValid;
@property (nonatomic, readonly) int gamepadId;
@property (nonatomic, readonly) RYString *name;
@property (nonatomic, readonly) const char *nameUTF8;
@property (nonatomic, readonly) RYGamepadType type;
@property (nonatomic, readonly) void *internalGamepad;

+ (instancetype)newGamepadWithApplication:(RYApplication *)application id:(int)gamepadId;
+ (instancetype)newGamepadWithApplication:(RYApplication *)application id:(int)gamepadId error:(RYError **)error;

- (instancetype)initWithApplication:(RYApplication *)application id:(int)gamepadId error:(RYError **)error;

/**
 * @brief Triggers standard dual-motor haptic rumble vibration.
 */
- (BOOL)setRumbleWithLowFrequency:(uint16_t)lowFreq
                    highFrequency:(uint16_t)highFreq
                         duration:(uint32_t)durationMs;

- (BOOL)setRumbleWithLowFrequency:(uint16_t)lowFreq
                    highFrequency:(uint16_t)highFreq
                         duration:(uint32_t)durationMs
                            error:(RYError **)error;

/**
 * @brief Triggers trigger motor rumble vibration.
 */
- (BOOL)setTriggerRumbleWithLeftTrigger:(uint16_t)leftTrigger
                          rightTrigger:(uint16_t)rightTrigger
                              duration:(uint32_t)durationMs;

- (BOOL)setTriggerRumbleWithLeftTrigger:(uint16_t)leftTrigger
                          rightTrigger:(uint16_t)rightTrigger
                              duration:(uint32_t)durationMs
                                 error:(RYError **)error;

@end
