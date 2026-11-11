#include "../../private.h"

@implementation RYMetalGraphicsBackend (Buffer)

- (void *)createBufferWithData:(const void *)data
                          size:(size_t)size
                          type:(RYBufferType)type
                         error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_internal) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                           message:"Metal backend is not initialized"];
        }
        return NULL;
    }

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    id<MTLBuffer> mtlBuffer = [intern->device newBufferWithBytes:data
                                                         length:size
                                                        options:MTLResourceStorageModeShared];

    if (!mtlBuffer) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsBufferCreationFailed
                                           message:"Failed to create MTLBuffer"];
        }
        return NULL;
    }

    RYInternalBuffer *buffer = new RYInternalBuffer;
    buffer->buffer = mtlBuffer;

    return buffer;
}

- (void)destroyBuffer:(void *)bufferHandle {
    if (!bufferHandle) return;

    RYInternalBuffer *buffer = (RYInternalBuffer *)bufferHandle;
    if (buffer->buffer) {
        [buffer->buffer release];
    }
    delete buffer;
}

@end
