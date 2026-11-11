#import "ryolib/graphics/RYTexture.h"
#import "ryolib/graphics/RYRenderer.h"

@implementation RYTexture

@synthesize renderer = _renderer;
@synthesize width = _width;
@synthesize height = _height;

+ (instancetype)newTextureWithRenderer:(RYRenderer *)renderer
                                pixels:(const void *)pixels
                                 width:(unsigned int)width
                                height:(unsigned int)height {
    return [self newTextureWithRenderer:renderer pixels:pixels width:width height:height error:NULL];
}

+ (instancetype)newTextureWithRenderer:(RYRenderer *)renderer
                                pixels:(const void *)pixels
                                 width:(unsigned int)width
                                height:(unsigned int)height
                                 error:(RYError **)error {
    return [[self alloc] initWithRenderer:renderer pixels:pixels width:width height:height error:error];
}

- (instancetype)initWithRenderer:(RYRenderer *)renderer
                          pixels:(const void *)pixels
                           width:(unsigned int)width
                          height:(unsigned int)height
                           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!renderer || !renderer.backendInstance) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid renderer passed to RYTexture"];
        }
        return nil;
    }

    void *textureHandle = [renderer.backendInstance createTextureWithPixels:pixels
                                                                      width:width
                                                                     height:height
                                                                      error:error];
    if (!textureHandle) {
        return nil;
    }

    self = [super init];
    if (!self) {
        [renderer.backendInstance destroyTexture:textureHandle];
        return nil;
    }

    _internal = textureHandle;
    _renderer = [renderer retain];
    _width = width;
    _height = height;

    return self;
}

- (instancetype)initWithInternalTexture:(void *)internalTexture
                               renderer:(RYRenderer *)renderer
                                  width:(unsigned int)width
                                 height:(unsigned int)height {
    self = [super init];
    if (!self) {
        return nil;
    }

    _internal = internalTexture;
    _renderer = [renderer retain];
    _width = width;
    _height = height;

    return self;
}

- (void)dealloc {
    if (_internal) {
        if (_renderer) {
            [_renderer.backendInstance destroyTexture:_internal];
        }
        _internal = NULL;
    }

    if (_renderer) {
        [_renderer release];
        _renderer = nil;
    }

    [super dealloc];
}

- (void *)internalTexture {
    return _internal;
}

@end
