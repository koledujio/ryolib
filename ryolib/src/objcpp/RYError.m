#import "ryolib/objcpp/RYError.h"

@implementation RYError

@synthesize code = _code;
@synthesize message = _message;

- (instancetype)initWithCode:(int)code
                     message:(RYString *)message {
    self = [super init];

    if (!self) {
        return nil;
    }

    if (!message) {
        [self release];
        return nil;
    }

    _code = code;
    _message = [message retain];

    return self;
}

- (void)dealloc {
    [_message release];
    [super dealloc];
}

@end