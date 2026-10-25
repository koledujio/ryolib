#import "ryolib/objcpp/RYString.h"

#include <stdlib.h>
#include <string.h>

@implementation RYString

@synthesize UTF8Length = _utf8Length;

- (const char *)UTF8String {
    return _utf8String;
}

- (instancetype)initWithUTF8String:(const char *)string {
    self = [super init];

    if (!self) {
        return nil;
    }
    
    if (!string) {
        [self release];
        return nil;
    }
    
    _utf8Length = (unsigned long)strlen(string);
    _utf8String = malloc(_utf8Length + 1);

    if (!_utf8String) {
        [self release];
        return nil;
    }

    memcpy(_utf8String, string, _utf8Length + 1);

    return self;
}

- (void)dealloc {
    free(_utf8String);
    [super dealloc];
}

@end
