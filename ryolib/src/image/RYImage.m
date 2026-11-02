#import "ryolib/image/RYImage.h"
#import "stb/ImageBackendStb.h"
#import "stub/ImageBackendStub.h"
#include <stdlib.h>

extern void _ry_image_texture_category_anchor(void);

@implementation RYImage

+ (void)initialize {
    if (self == [RYImage class]) {
        _ry_image_texture_category_anchor();
    }
}

@synthesize path = _path;
@synthesize width = _width;
@synthesize height = _height;
@synthesize channels = _channels;
@synthesize pixels = _pixels;
@synthesize backendInstance = _backendInstance;

+ (Class)backendClassForType:(RYImageBackendType)type {
    switch (type) {
        case RYImageBackendTypeDefault:
#if defined(RYO_ENABLE_IMAGE_STB)
            return [RYImageBackendStb class];
#else
            return [RYImageBackendStub class];
#endif
        case RYImageBackendTypeStb:
#if defined(RYO_ENABLE_IMAGE_STB)
            return [RYImageBackendStb class];
#else
            return Nil;
#endif
        case RYImageBackendTypeStub:
            return [RYImageBackendStub class];
        default:
            return Nil;
    }
}

+ (instancetype)newImageWithPath:(RYString *)path {
    return [self newImageWithPath:path backend:RYImageBackendTypeDefault error:NULL];
}

+ (instancetype)newImageWithPath:(RYString *)path error:(RYError **)error {
    return [self newImageWithPath:path backend:RYImageBackendTypeDefault error:error];
}

+ (instancetype)newImageWithPath:(RYString *)path
                         backend:(RYImageBackendType)backendType
                           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!path) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageInvalidPath
                                           message:"Image path is nil"];
        }
        return nil;
    }

    Class backendClass = [self backendClassForType:backendType];
    if (backendClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageInvalidBackend
                                           message:"Requested image backend is unsupported or invalid"];
        }
        return nil;
    }

    RYImageBackend *backend = [[backendClass alloc] initBackendWithError:error];
    if (!backend) {
        return nil;
    }

    int width = 0;
    int height = 0;
    int channels = 0;
    void *pixels = [backend loadImageWithPath:path
                                     outWidth:&width
                                    outHeight:&height
                                  outChannels:&channels
                                        error:error];
    if (!pixels) {
        [backend release];
        return nil;
    }

    RYImage *image = [[self alloc] initWithPixels:pixels
                                            width:width
                                           height:height
                                         channels:channels
                                     freeWhenDone:YES
                                  backendInstance:backend];
    if (image) {
        image->_path = [path retain];
    }
    [backend release];
    return image;
}

+ (instancetype)newImageWithMemory:(const void *)memory size:(unsigned long)size {
    return [self newImageWithMemory:memory size:size backend:RYImageBackendTypeDefault error:NULL];
}

+ (instancetype)newImageWithMemory:(const void *)memory size:(unsigned long)size error:(RYError **)error {
    return [self newImageWithMemory:memory size:size backend:RYImageBackendTypeDefault error:error];
}

+ (instancetype)newImageWithMemory:(const void *)memory
                              size:(unsigned long)size
                           backend:(RYImageBackendType)backendType
                             error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!memory || size == 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageInvalidMemory
                                           message:"Memory pointer is null or size is zero"];
        }
        return nil;
    }

    Class backendClass = [self backendClassForType:backendType];
    if (backendClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageInvalidBackend
                                           message:"Requested image backend is unsupported or invalid"];
        }
        return nil;
    }

    RYImageBackend *backend = [[backendClass alloc] initBackendWithError:error];
    if (!backend) {
        return nil;
    }

    int width = 0;
    int height = 0;
    int channels = 0;
    void *pixels = [backend loadImageWithMemory:memory
                                           size:size
                                       outWidth:&width
                                      outHeight:&height
                                    outChannels:&channels
                                          error:error];
    if (!pixels) {
        [backend release];
        return nil;
    }

    RYImage *image = [[self alloc] initWithPixels:pixels
                                            width:width
                                           height:height
                                         channels:channels
                                     freeWhenDone:YES
                                  backendInstance:backend];
    [backend release];
    return image;
}

- (instancetype)initWithPixels:(void *)pixels
                         width:(int)width
                        height:(int)height
                      channels:(int)channels
                  freeWhenDone:(BOOL)freeWhenDone {
    return [self initWithPixels:pixels
                          width:width
                         height:height
                       channels:channels
                   freeWhenDone:freeWhenDone
                backendInstance:nil];
}

- (instancetype)initWithPixels:(void *)pixels
                         width:(int)width
                        height:(int)height
                      channels:(int)channels
                  freeWhenDone:(BOOL)freeWhenDone
               backendInstance:(RYImageBackend *)backendInstance {
    self = [super init];
    if (self) {
        _path = nil;
        _width = width;
        _height = height;
        _channels = channels;
        _pixels = pixels;
        _ownsPixels = freeWhenDone;
        _backendInstance = [backendInstance retain];
    }
    return self;
}

- (void)dealloc {
    if (_ownsPixels && _pixels) {
        if (_backendInstance) {
            [_backendInstance freePixels:(void *)_pixels];
        } else {
            free((void *)_pixels);
        }
        _pixels = NULL;
    }
    [_backendInstance release];
    [_path release];
    [super dealloc];
}

@end
