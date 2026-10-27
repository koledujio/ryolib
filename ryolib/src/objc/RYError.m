#import "ryolib/objc/RYError.h"

@implementation RYError

@synthesize code = _code;
@synthesize message = _message;

- (instancetype)init {
    return [self initWithCode:RYErrorCodeUnknown message:"No error message provided"];
}

- (instancetype)initWithCode:(RYErrorCode)code
                     message:(const char *)message {
    self = [super init];

    if (!self) {
        return nil;
    }

    if (!message) {
        [self release];
        return nil;
    }

    _code = code;
    _message = [[RYString alloc] initWithUTF8String:message];

    if (!_message) {
        [self release];
        return nil;
    }

    return self;
}

- (void)dealloc {
    [_message release];
    [super dealloc];
}

@end
