#import "TextBackendStub.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

@implementation RYTextBackendStub

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
        *error = nil;
    }
    return (void *)0x1;
}

- (void *)createFontWithMemory:(const void *)memory
                          size:(unsigned long)size
                      fontSize:(float)fontSize
                         error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    return (void *)0x1;
}

- (void)destroyFont:(void *)fontHandle {
}

- (BOOL)addFallbackFont:(void *)fallbackHandle
                 toFont:(void *)fontHandle
                  error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    return YES;
}

- (BOOL)removeFallbackFont:(void *)fallbackHandle
                  fromFont:(void *)fontHandle
                     error:(RYError **)error {
    if (error) {
        *error = nil;
    }
    return YES;
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
        *error = nil;
    }

    if (outWidth) *outWidth = 1;
    if (outHeight) *outHeight = 1;

    uint32_t *pixels = (uint32_t *)malloc(sizeof(uint32_t));
    if (pixels) {
        *pixels = 0xFFFFFFFF;
    }
    return pixels;
}

- (void)freePixels:(void *)pixels {
    if (pixels) {
        free(pixels);
    }
}

@end
