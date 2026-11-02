#if defined(RYO_ENABLE_IMAGE_STB)

#import "ImageBackendStb.h"
#include "stb_image.h"

@implementation RYImageBackendStb

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

    if (!path || !path.UTF8String) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageInvalidPath
                                           message:"Image path is null or empty"];
        }
        return NULL;
    }

    int localChannels = 0;
    int localWidth = 0;
    int localHeight = 0;

    // Load with 4 desired channels (RGBA)
    unsigned char *pixels = stbi_load(path.UTF8String, &localWidth, &localHeight, &localChannels, 4);
    if (!pixels) {
        if (error) {
            const char *reason = stbi_failure_reason();
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageLoadFailed
                                           message:reason ? reason : "stbi_load failed"];
        }
        return NULL;
    }

    if (width) *width = localWidth;
    if (height) *height = localHeight;
    if (channels) *channels = 4;

    return pixels;
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

    if (!memory || size == 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageInvalidMemory
                                           message:"Memory buffer is null or size is 0"];
        }
        return NULL;
    }

    int localChannels = 0;
    int localWidth = 0;
    int localHeight = 0;

    unsigned char *pixels = stbi_load_from_memory((const stbi_uc *)memory, (int)size, &localWidth, &localHeight, &localChannels, 4);
    if (!pixels) {
        if (error) {
            const char *reason = stbi_failure_reason();
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageLoadFailed
                                           message:reason ? reason : "stbi_load_from_memory failed"];
        }
        return NULL;
    }

    if (width) *width = localWidth;
    if (height) *height = localHeight;
    if (channels) *channels = 4;

    return pixels;
}

- (void)freePixels:(void *)pixels {
    if (pixels) {
        stbi_image_free(pixels);
    }
}

@end

#endif
