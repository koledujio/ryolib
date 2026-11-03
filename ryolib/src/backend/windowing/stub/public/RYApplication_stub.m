#import "ryolib/windowing/RYApplication.h"

@implementation RYApplication

@synthesize isValid = _isValid;
@synthesize capabilities = _capabilities;
@synthesize internalApplication = _internal;

+ (instancetype)application {
    return [self applicationWithError:NULL];
}

+ (instancetype)applicationWithError:(RYError **)error {
    return [[self alloc] initWithError:error];
}

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
    if (self) {
        _isValid = NO;
        _internal = NULL;
    }
    return self;
}

- (BOOL)pollEvent:(RYEvent *)event {
    return NO;
}

@end
