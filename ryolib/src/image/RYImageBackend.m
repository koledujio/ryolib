#import "ryolib/image/RYImageBackend.h"

@implementation RYImageBackend

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
        *error = [[RYError alloc] initWithCode:RYErrorCodeImageBackendFeatureNotSupported
                                       message:"Backend does not support loading images from file path"];
    }
    return NULL;
}

- (void *)loadImageWithMemory:(const void *)memory
                         size:(unsigned long)size
                     outWidth:(int *)width
                    outHeight:(int *)height
                  outChannels:(int *)channels
                        error:(RYError **)error {
    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeImageBackendFeatureNotSupported
                                       message:"Backend does not support loading images from memory"];
    }
    return NULL;
}

- (void)freePixels:(void *)pixels {
}

@end
