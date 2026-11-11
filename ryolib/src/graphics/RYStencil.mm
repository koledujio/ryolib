#import "ryolib/graphics/RYStencil.h"
#import "ryolib/graphics/RYRenderer.h"

@implementation RYStencil

@synthesize renderer = _renderer;

+ (instancetype)newStencilWithRenderer:(RYRenderer *)renderer {
    return [self newStencilWithRenderer:renderer error:NULL];
}

+ (instancetype)newStencilWithRenderer:(RYRenderer *)renderer error:(RYError **)error {
    return [[self alloc] initWithRenderer:renderer error:error];
}

- (instancetype)initWithRenderer:(RYRenderer *)renderer error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!renderer || !renderer.backendInstance) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid renderer passed to RYStencil"];
        }
        return nil;
    }

    void *stencilHandle = [renderer.backendInstance createStencilWithError:error];
    if (!stencilHandle) {
        return nil;
    }

    self = [super init];
    if (!self) {
        [renderer.backendInstance destroyStencil:stencilHandle];
        return nil;
    }

    _internal = stencilHandle;
    _renderer = [renderer retain];

    return self;
}

- (void)dealloc {
    if (_internal) {
        if (_renderer) {
            [_renderer.backendInstance destroyStencil:_internal];
        }
        _internal = NULL;
    }

    if (_renderer) {
        [_renderer release];
        _renderer = nil;
    }

    [super dealloc];
}

- (BOOL)isEmpty {
    if (!_internal) return YES;
    return [_renderer.backendInstance stencilIsEmpty:_internal];
}

- (void *)internalStencil {
    return _internal;
}

- (BOOL)clear {
    if (!_internal) return NO;
    return [_renderer.backendInstance clearStencil:_internal];
}

- (BOOL)drawRectangle:(ryo::Matrix4)matrix camera:(const RYCamera *)camera {
    if (!_internal) return NO;
    return [_renderer.backendInstance addRectangleToStencil:_internal matrix:matrix camera:camera];
}

- (BOOL)drawRoundedRectangle:(ryo::Matrix4)matrix radius:(float)radius camera:(const RYCamera *)camera {
    if (!_internal) return NO;
    return [_renderer.backendInstance addRoundedRectangleToStencil:_internal
                                                            matrix:matrix
                                                            radius:radius
                                                            camera:camera];
}

- (BOOL)drawOval:(ryo::Matrix4)matrix camera:(const RYCamera *)camera {
    if (!_internal) return NO;
    return [_renderer.backendInstance addOvalToStencil:_internal matrix:matrix camera:camera];
}

@end
