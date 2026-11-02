#import "ImageBackendStub.h"
#include <stdlib.h>
#include <string.h>

@implementation RYImageBackendStub

- (instancetype)initBackendWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }
    return [super init];
}

- (void *)loadImageWithPath:(RYString *)path
                   outWidth:(int *)width
                  outHeight:(int *)height
                outChannels:(int *)channels
                      error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (width) *width = 1;
    if (height) *height = 1;
    if (channels) *channels = 4;

    unsigned char *pixel = (unsigned char *)malloc(4);
    if (pixel) {
        pixel[0] = 255;
        pixel[1] = 0;
        pixel[2] = 255;
        pixel[3] = 255;
    }
    return pixel;
}

- (void *)loadImageWithMemory:(const void *)memory
                         size:(unsigned long)size
                     outWidth:(int *)width
                    outHeight:(int *)height
                  outChannels:(int *)channels
                        error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (width) *width = 1;
    if (height) *height = 1;
    if (channels) *channels = 4;

    unsigned char *pixel = (unsigned char *)malloc(4);
    if (pixel) {
        pixel[0] = 255;
        pixel[1] = 0;
        pixel[2] = 255;
        pixel[3] = 255;
    }
    return pixel;
}

- (void)freePixels:(void *)pixels {
    if (pixels) {
        free(pixels);
    }
}

@end
