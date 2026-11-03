#import "ryolib/windowing/RYGamepad.h"

@implementation RYGamepad

@synthesize application = _application;
@synthesize gamepadId = _gamepadId;
@synthesize type = _type;
@synthesize name = _name;
@synthesize internalGamepad = _internal;

+ (instancetype)newGamepadWithApplication:(RYApplication *)application id:(int)gamepadId {
    return [[self alloc] initWithApplication:application id:gamepadId error:NULL];
}

+ (instancetype)newGamepadWithApplication:(RYApplication *)application id:(int)gamepadId error:(RYError **)error {
    return [[self alloc] initWithApplication:application id:gamepadId error:error];
}

- (instancetype)initWithApplication:(RYApplication *)application id:(int)gamepadId error:(RYError **)error {
    self = [super init];
    if (self) {
        _internal = NULL;
        _gamepadId = -1;
        _type = RYGamepadTypeUnknown;
    }
    return self;
}

- (BOOL)isValid { return NO; }
- (const char *)nameUTF8 { return NULL; }

- (BOOL)setRumbleWithLowFrequency:(uint16_t)lowFreq highFrequency:(uint16_t)highFreq duration:(uint32_t)durationMs { return NO; }
- (BOOL)setRumbleWithLowFrequency:(uint16_t)lowFreq highFrequency:(uint16_t)highFreq duration:(uint32_t)durationMs error:(RYError **)error { return NO; }

- (BOOL)setTriggerRumbleWithLeftTrigger:(uint16_t)leftTrigger rightTrigger:(uint16_t)rightTrigger duration:(uint32_t)durationMs { return NO; }
- (BOOL)setTriggerRumbleWithLeftTrigger:(uint16_t)leftTrigger rightTrigger:(uint16_t)rightTrigger duration:(uint32_t)durationMs error:(RYError **)error { return NO; }

@end
