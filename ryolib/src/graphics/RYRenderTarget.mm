#import "ryolib/graphics/RYRenderTarget.h"
#import "ryolib/graphics/RYRenderer.h"

@implementation RYRenderTarget

@synthesize renderer = _renderer;
@synthesize colorTexture = _colorTexture;
@synthesize depthTexture = _depthTexture;
@synthesize width = _width;
@synthesize height = _height;

+ (instancetype)newRenderTargetWithRenderer:(RYRenderer *)renderer
                                      width:(unsigned int)width
                                     height:(unsigned int)height {
    return [self newRenderTargetWithRenderer:renderer width:width height:height error:NULL];
}

+ (instancetype)newRenderTargetWithRenderer:(RYRenderer *)renderer
                                      width:(unsigned int)width
                                     height:(unsigned int)height
                                      error:(RYError **)error {
    return [[self alloc] initWithRenderer:renderer width:width height:height error:error];
}

- (instancetype)initWithRenderer:(RYRenderer *)renderer
                           width:(unsigned int)width
                          height:(unsigned int)height
                           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!renderer || !renderer.backendInstance) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid renderer passed to RYRenderTarget"];
        }
        return nil;
    }

    void *colorHandle = NULL;
    void *depthHandle = NULL;
    void *targetHandle = [renderer.backendInstance createRenderTargetWithWidth:width
                                                                        height:height
                                                                  colorTexture:&colorHandle
                                                                  depthTexture:&depthHandle
                                                                         error:error];
    if (!targetHandle) {
        return nil;
    }

    self = [super init];
    if (!self) {
        [renderer.backendInstance destroyTexture:colorHandle];
        [renderer.backendInstance destroyTexture:depthHandle];
        [renderer.backendInstance destroyRenderTarget:targetHandle];
        return nil;
    }

    _internal = targetHandle;
    _renderer = [renderer retain];
    _width = width;
    _height = height;

    _colorTexture = [[RYTexture alloc] initWithInternalTexture:colorHandle
                                                      renderer:renderer
                                                         width:width
                                                        height:height];
    _depthTexture = [[RYTexture alloc] initWithInternalTexture:depthHandle
                                                      renderer:renderer
                                                         width:width
                                                        height:height];

    if (!_colorTexture || !_depthTexture) {
        [self release];
        return nil;
    }

    return self;
}

- (void)dealloc {
    if (_colorTexture) {
        [_colorTexture release];
        _colorTexture = nil;
    }

    if (_depthTexture) {
        [_depthTexture release];
        _depthTexture = nil;
    }

    if (_internal) {
        if (_renderer) {
            [_renderer.backendInstance destroyRenderTarget:_internal];
        }
        _internal = NULL;
    }

    if (_renderer) {
        [_renderer release];
        _renderer = nil;
    }

    [super dealloc];
}

- (void *)internalRenderTarget {
    return _internal;
}

@end
