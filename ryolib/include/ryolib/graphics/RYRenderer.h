#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYGraphicsTypes.h"
#import "RYTexture.h"
#import "RYBuffer.h"
#import "RYPipeline.h"
#import "RYStencil.h"
#import "RYRenderTarget.h"
#import "RYGraphicsBackend.h"

/**
 * @brief Creates GPU resources and submits draw calls.
 *
 * A renderer owns a graphics backend and the built-in pipelines. The backend
 * is chosen at creation time and can be replaced with a custom implementation.
 * Frame and draw helpers are grouped into categories below.
 */
@interface RYRenderer : RYObject {
@protected
    RYGraphicsBackendType _backend;
    RYGraphicsBackend *_backendInstance;
    RYPipeline *_builtinPipelines[3];
}

/** Backend used by the renderer. */
@property (nonatomic, readonly) RYGraphicsBackendType backend;

/** Backend instance owned by the renderer. */
@property (nonatomic, readonly) RYGraphicsBackend *backendInstance;

/** Creates a renderer with the default backend. Returns nil on failure. */
+ (instancetype)newRendererWithConfig:(const RYRendererConfig *)config;

/** Creates a renderer with the default backend and reports errors. */
+ (instancetype)newRendererWithConfig:(const RYRendererConfig *)config error:(RYError **)error;

/** Creates a renderer with the default backend, sharing another renderer's device. */
+ (instancetype)newRendererWithConfig:(const RYRendererConfig *)config shareRenderer:(RYRenderer *)shareRenderer error:(RYError **)error;

/** Creates a renderer with the given backend. Returns nil on failure. */
+ (instancetype)newRendererWithBackend:(RYGraphicsBackendType)backend
                                config:(const RYRendererConfig *)config;

/** Creates a renderer with the given backend and reports errors. */
+ (instancetype)newRendererWithBackend:(RYGraphicsBackendType)backend
                                config:(const RYRendererConfig *)config
                                 error:(RYError **)error;

/** Creates a renderer with the given backend, sharing another renderer's device. */
+ (instancetype)newRendererWithBackend:(RYGraphicsBackendType)backend
                                config:(const RYRendererConfig *)config
                         shareRenderer:(RYRenderer *)shareRenderer
                                 error:(RYError **)error;

/** Creates a renderer, which retains and uses the specified custom graphics backend. */
+ (instancetype)newRendererWithCustomBackend:(RYGraphicsBackend *)customBackend;

/** Creates a renderer, which retains and uses the specified custom graphics backend, reports errors. */
+ (instancetype)newRendererWithCustomBackend:(RYGraphicsBackend *)customBackend
                                       error:(RYError **)error;

/** Designated initializer. */
- (instancetype)initWithBackendInstance:(RYGraphicsBackend *)backendInstance
                            backendType:(RYGraphicsBackendType)backend;

/** Resizes the renderer's framebuffer. */
- (BOOL)resizeWithWidth:(unsigned int)width height:(unsigned int)height;

/** Returns the framebuffer size in pixels. */
- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight;

/** Returns the content scale used for pixel-perfect drawing. */
- (BOOL)getContentScale:(float *)outScale;

/** Sets the content scale used for pixel-perfect drawing. */
- (BOOL)setContentScale:(float)scale;

/** Returns one of the renderer's built-in pipelines. */
- (RYPipeline *)builtinPipeline:(RYBuiltinPipeline)pipeline;

@end

/** Frame lifecycle. */
@interface RYRenderer (Frame)

/** Starts a frame and acquires the next drawable. */
- (void)frameBegin;

/** Presents the frame. Returns NO if no frame is active. */
- (BOOL)frameEnd;

/** Time in seconds since the previous frame. */
- (float)frameTime;

@end

/** Render pass setup. */
@interface RYRenderer (DrawPass)

/** Begins a pass targeting the given render target. Pass nil to draw to the window. */
- (void)drawPassBeginWithRenderTarget:(RYRenderTarget *)renderTarget clearColor:(const RYColor *)clearColor;

/** Ends the current render pass. */
- (void)drawPassEnd;

@end

/** Low-level draw submission. */
@interface RYRenderer (DrawCall)

/** Submits a fully described draw call. */
- (BOOL)drawEx:(const RYDrawCall *)drawCall;

@end

/** Built-in solid color quad drawing. */
@interface RYRenderer (RectColor)

/** Draws a colored quad with the given transform. */
- (BOOL)drawRectColor:(RYColor)color matrix:(ryo::Matrix4)matrix settings:(const RYDrawSettings *)settings;

@end

/** Built-in textured quad drawing. */
@interface RYRenderer (RectTexture)

/** Draws a textured quad with the given transform. */
- (BOOL)drawRectTexture:(RYTexture *)texture samplerType:(RYSamplerType)samplerType color:(RYColor)color matrix:(ryo::Matrix4)matrix settings:(const RYDrawSettings *)settings;

@end

/** Built-in textured mesh drawing. */
@interface RYRenderer (MeshTexture)

/** Draws an indexed triangle mesh with a single texture. */
- (BOOL)drawMeshTextureWithVertexBuffer:(RYBuffer *)vertexBuffer indexBuffer:(RYBuffer *)indexBuffer indexCount:(uint32_t)indexCount texture:(RYTexture *)texture samplerType:(RYSamplerType)samplerType color:(RYColor)color matrix:(ryo::Matrix4)matrix settings:(const RYDrawSettings *)settings;

@end
