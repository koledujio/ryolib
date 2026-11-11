#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYGraphicsTypes.h"
#import "RYTexture.h"

/**
 * @brief An offscreen color and depth/stencil target.
 *
 * The renderer can draw into a render target, then the color texture can be
 * sampled later like any other texture.
 */
@interface RYRenderTarget : RYObject {
@protected
    void *_internal;
    RYRenderer *_renderer;
    RYTexture *_colorTexture;
    RYTexture *_depthTexture;
    unsigned int _width;
    unsigned int _height;
}

/** Renderer the target was created on. */
@property (nonatomic, readonly) RYRenderer *renderer;

/** Color texture written by the target. */
@property (nonatomic, readonly) RYTexture *colorTexture;

/** Depth and stencil texture used by the target. */
@property (nonatomic, readonly) RYTexture *depthTexture;

/** Width in pixels. */
@property (nonatomic, readonly) unsigned int width;

/** Height in pixels. */
@property (nonatomic, readonly) unsigned int height;

/** Backend-specific render target state. Do not free this pointer yourself. */
@property (nonatomic, readonly) void *internalRenderTarget;

/** Creates a render target. Returns nil on failure. */
+ (instancetype)newRenderTargetWithRenderer:(RYRenderer *)renderer
                                      width:(unsigned int)width
                                     height:(unsigned int)height;

/** Creates a render target and reports errors. */
+ (instancetype)newRenderTargetWithRenderer:(RYRenderer *)renderer
                                      width:(unsigned int)width
                                     height:(unsigned int)height
                                      error:(RYError **)error;

/** Designated initializer. */
- (instancetype)initWithRenderer:(RYRenderer *)renderer
                           width:(unsigned int)width
                          height:(unsigned int)height
                           error:(RYError **)error;

@end
