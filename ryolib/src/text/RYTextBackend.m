#import "ryolib/text/RYTextBackend.h"

@implementation RYTextBackend

- (instancetype)initBackendWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }
    return [super init];
}

- (void *)createFontWithPath:(RYString *)path
                    fontSize:(float)fontSize
                       error:(RYError **)error {
    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeTextBackendFeatureNotSupported
                                       message:"Backend does not support creating fonts from file path"];
    }
    return NULL;
}

- (void *)createFontWithMemory:(const void *)memory
                          size:(unsigned long)size
                      fontSize:(float)fontSize
                         error:(RYError **)error {
    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeTextBackendFeatureNotSupported
                                       message:"Backend does not support creating fonts from memory"];
    }
    return NULL;
}

- (void)destroyFont:(void *)fontHandle {
}

- (BOOL)addFallbackFont:(void *)fallbackHandle
                 toFont:(void *)fontHandle
                  error:(RYError **)error {
    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeTextBackendFeatureNotSupported
                                       message:"Backend does not support fallback fonts"];
    }
    return NO;
}

- (BOOL)removeFallbackFont:(void *)fallbackHandle
                  fromFont:(void *)fontHandle
                     error:(RYError **)error {
    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeTextBackendFeatureNotSupported
                                       message:"Backend does not support removing fallback fonts"];
    }
    return NO;
}

- (void)clearFallbackFonts:(void *)fontHandle {
}

- (void *)renderLine:(RYString *)text
                font:(void *)fontHandle
               color:(RYColor)color
            outWidth:(int *)outWidth
           outHeight:(int *)outHeight
               error:(RYError **)error {
    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeTextBackendFeatureNotSupported
                                       message:"Backend does not support text rendering"];
    }
    return NULL;
}

- (void)freePixels:(void *)pixels {
}

@end
