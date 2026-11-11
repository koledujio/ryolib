#import "ryolib/graphics/RYBuffer.h"
#import "ryolib/graphics/RYRenderer.h"

@implementation RYBuffer

@synthesize renderer = _renderer;
@synthesize type = _type;
@synthesize size = _size;

+ (instancetype)newBufferWithRenderer:(RYRenderer *)renderer
                                 data:(const void *)data
                                 size:(size_t)size
                                 type:(RYBufferType)type {
    return [self newBufferWithRenderer:renderer data:data size:size type:type error:NULL];
}

+ (instancetype)newBufferWithRenderer:(RYRenderer *)renderer
                                 data:(const void *)data
                                 size:(size_t)size
                                 type:(RYBufferType)type
                                error:(RYError **)error {
    return [[self alloc] initWithRenderer:renderer data:data size:size type:type error:error];
}

- (instancetype)initWithRenderer:(RYRenderer *)renderer
                            data:(const void *)data
                            size:(size_t)size
                            type:(RYBufferType)type
                           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!renderer || !renderer.backendInstance) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid renderer passed to RYBuffer"];
        }
        return nil;
    }

    void *bufferHandle = [renderer.backendInstance createBufferWithData:data
                                                                  size:size
                                                                  type:type
                                                                 error:error];
    if (!bufferHandle) {
        return nil;
    }

    self = [super init];
    if (!self) {
        [renderer.backendInstance destroyBuffer:bufferHandle];
        return nil;
    }

    _internal = bufferHandle;
    _renderer = [renderer retain];
    _type = type;
    _size = size;
    _ownsInternal = YES;

    return self;
}

- (instancetype)initWithInternalBuffer:(void *)internalBuffer
                              renderer:(RYRenderer *)renderer
                                  type:(RYBufferType)type
                                  size:(size_t)size {
    self = [super init];
    if (!self) {
        return nil;
    }

    _internal = internalBuffer;
    _renderer = [renderer retain];
    _type = type;
    _size = size;
    _ownsInternal = NO;

    return self;
}

- (void)dealloc {
    if (_internal) {
        if (_ownsInternal && _renderer) {
            [_renderer.backendInstance destroyBuffer:_internal];
        }
        _internal = NULL;
    }

    if (_renderer) {
        [_renderer release];
        _renderer = nil;
    }

    [super dealloc];
}

- (void *)internalBuffer {
    return _internal;
}

@end
