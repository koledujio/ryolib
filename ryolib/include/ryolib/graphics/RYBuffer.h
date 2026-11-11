#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYGraphicsTypes.h"

/**
 * @brief A GPU vertex or index buffer.
 *
 * The data is copied into the buffer at creation time, so the caller can free
 * its copy right after the call.
 */
@interface RYBuffer : RYObject {
@protected
    void *_internal;
    RYRenderer *_renderer;
    RYBufferType _type;
    size_t _size;
    BOOL _ownsInternal;
}

/** Renderer the buffer was created on. */
@property (nonatomic, readonly) RYRenderer *renderer;

/** Buffer type (vertex or index). */
@property (nonatomic, readonly) RYBufferType type;

/** Size in bytes. */
@property (nonatomic, readonly) size_t size;

/** Backend-specific buffer state. Do not free this pointer yourself. */
@property (nonatomic, readonly) void *internalBuffer;

/** Creates a buffer and copies the given data into it. Returns nil on failure. */
+ (instancetype)newBufferWithRenderer:(RYRenderer *)renderer
                                 data:(const void *)data
                                 size:(size_t)size
                                 type:(RYBufferType)type;

/** Creates a buffer and reports errors. */
+ (instancetype)newBufferWithRenderer:(RYRenderer *)renderer
                                 data:(const void *)data
                                 size:(size_t)size
                                 type:(RYBufferType)type
                                error:(RYError **)error;

/** Designated initializer. */
- (instancetype)initWithRenderer:(RYRenderer *)renderer
                            data:(const void *)data
                            size:(size_t)size
                            type:(RYBufferType)type
                           error:(RYError **)error;

/**
 * Wraps an existing backend buffer.
 *
 * Borrows the internal buffer; the backend owns it and releases it when the
 * backend is deallocated. Used for the renderer's built-in buffers, not by
 * application code.
 */
- (instancetype)initWithInternalBuffer:(void *)internalBuffer
                              renderer:(RYRenderer *)renderer
                                  type:(RYBufferType)type
                                  size:(size_t)size;

@end
