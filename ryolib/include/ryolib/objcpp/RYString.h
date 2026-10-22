#pragma once

#import "RYObject.h"

@interface RYString : RYObject {
@private
    char *_utf8String;
    unsigned long _utf8Length;
}

@property (nonatomic, readonly) const char *UTF8String;
@property (nonatomic, readonly) unsigned long UTF8Length;

- (instancetype)initWithUTF8String:(const char *)string;

@end