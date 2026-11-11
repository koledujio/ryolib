#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYGraphicsTypes.h"

/**
 * @brief A GPU texture created from raw pixel data.
 *
 * The texture is uploaded once at creation time.
 */
@interface RYTexture : RYObject {
@protected
    void *_internal;
    RYRenderer *_renderer;
    unsigned int _width;
    unsigned int _height;
}

/** Renderer the texture was created on. */
@property (nonatomic, readonly) RYRenderer *renderer;

/** Width in pixels. */
@property (nonatomic, readonly) unsigned int width;

/** Height in pixels. */
@property (nonatomic, readonly) unsigned int height;

/** Backend-specific texture state. Do not free this pointer yourself. */
@property (nonatomic, readonly) void *internalTexture;

/** Creates a texture from RGBA pixel data. Returns nil on failure. */
+ (instancetype)newTextureWithRenderer:(RYRenderer *)renderer
                                pixels:(const void *)pixels
                                 width:(unsigned int)width
                                height:(unsigned int)height;

/** Creates a texture from RGBA pixel data and reports errors. */
+ (instancetype)newTextureWithRenderer:(RYRenderer *)renderer
                                pixels:(const void *)pixels
                                 width:(unsigned int)width
                                height:(unsigned int)height
                                 error:(RYError **)error;

/** Designated initializer. */
- (instancetype)initWithRenderer:(RYRenderer *)renderer
                          pixels:(const void *)pixels
                           width:(unsigned int)width
                          height:(unsigned int)height
                           error:(RYError **)error;

/**
 * Wraps an existing backend texture.
 *
 * Takes ownership of the internal texture and releases it when the object is
 * deallocated. Used by render targets, not by application code.
 */
- (instancetype)initWithInternalTexture:(void *)internalTexture
                               renderer:(RYRenderer *)renderer
                                  width:(unsigned int)width
                                 height:(unsigned int)height;

@end
