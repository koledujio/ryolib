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

    RYErrorCodeImageInvalidPath                 = 300,
    RYErrorCodeImageInvalidMemory               = 301,
    RYErrorCodeImageLoadFailed                  = 302,
    RYErrorCodeImageInvalidBackend              = 303,
    RYErrorCodeImageBackendInitializationFailed = 304,
    RYErrorCodeImageBackendPlatformNotSupported = 305,
    RYErrorCodeImageBackendFeatureNotSupported  = 306,
    RYErrorCodeImageBackendError                = 307,
    RYErrorCodeImageTextureCreationFailed       = 308,

    RYErrorCodeTextInvalidFontPath              = 400,
    RYErrorCodeTextInvalidFontSize              = 401,
    RYErrorCodeTextInvalidMemory                = 402,
    RYErrorCodeTextLoadFontFailed               = 403,
    RYErrorCodeTextInvalidBackend               = 404,
    RYErrorCodeTextBackendInitializationFailed  = 405,
    RYErrorCodeTextBackendPlatformNotSupported  = 406,
    RYErrorCodeTextBackendFeatureNotSupported   = 407,
    RYErrorCodeTextBackendError                 = 408,
    RYErrorCodeTextRenderFailed                 = 409,
    RYErrorCodeTextFallbackFailed               = 410,

    RYErrorCodeWindowingGeneralError            = 500,
    RYErrorCodeWindowingInitializationFailed    = 501,
    RYErrorCodeWindowingWindowCreationFailed    = 502,
    RYErrorCodeWindowingInvalidWindow           = 503,
    RYErrorCodeWindowingInvalidApplication      = 504,
    RYErrorCodeWindowingGamepadCreationFailed   = 505,
    RYErrorCodeWindowingInvalidGamepad          = 506,
    RYErrorCodeWindowingFeatureNotSupported     = 507,
    RYErrorCodeWindowingInvalidSize             = 508,
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
