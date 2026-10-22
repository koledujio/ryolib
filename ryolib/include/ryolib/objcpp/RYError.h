#pragma once

#import "RYObject.h"
#import "RYString.h"

@interface RYError : RYObject {
@private
    int _code;
    RYString *_message;
}

@property (nonatomic, readonly) int code;
@property (nonatomic, readonly) RYString *message;

- (instancetype)initWithCode:(int)code
                     message:(RYString *)message;

@end