#pragma once

#import "RYObject.h"
#import "RYString.h"

typedef enum {
    RYErrorCodeUnknown = 0,

    RYErrorCodeGeneralInvalidArgument = 100,
    RYErrorCodeGeneralInitFailed      = 101,
    RYErrorCodeGeneralNotInitialized  = 102,
    RYErrorCodeGeneralOutOfMemory     = 103,

    RYErrorCodeAudioInvalidBackend              = 200,
    RYErrorCodeAudioInvalidVolume               = 201,
    RYErrorCodeAudioInvalidAudioEngine          = 202,
    RYErrorCodeAudioInvalidSoundPath            = 203,
    RYErrorCodeAudioInvalidSeekTime             = 204,
    RYErrorCodeAudioBackendInitializationFailed = 206,
    RYErrorCodeAudioBackendPlatformNotSupported = 207,
    RYErrorCodeAudioBackendFeatureNotSupported  = 208,
    RYErrorCodeAudioBackendError                = 209,
    RYErrorCodeAudioBackendLoadAudioFailed      = 210,
} RYErrorCode;

@interface RYError : RYObject {
@private
    RYErrorCode _code;
    RYString *_message;
}

@property (nonatomic, readonly) RYErrorCode code;
@property (nonatomic, readonly) RYString *message;

- (instancetype)initWithCode:(RYErrorCode)code
                     message:(const char *)message;

@end
