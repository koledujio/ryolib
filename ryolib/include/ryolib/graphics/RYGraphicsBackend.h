#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYGraphicsTypes.h"

@class RYTexture;
@class RYBuffer;
@class RYRenderTarget;

/**
 * Identifies the graphics backend used by a renderer.
 */
typedef enum {
    RYGraphicsBackendDefault,
    RYGraphicsBackendStub,
    RYGraphicsBackendMetal,
    RYGraphicsBackendD3D11,
    RYGraphicsBackendGxm,
    RYGraphicsBackendCustom
} RYGraphicsBackendType;

/**
 * @brief Base class for graphics backends.
 *
 * A backend implements GPU resource creation and drawing for a specific
 * graphics API. Renderers choose a backend at creation time.
 */
@interface RYGraphicsBackend : RYObject {
@protected
    void *_internal;
}

/** Creates a backend for the given renderer configuration. */
- (instancetype)initBackendWithConfig:(const RYRendererConfig *)config
                         shareBackend:(RYGraphicsBackend *)shareBackend
                                error:(RYError **)error;

/** Backend type reported by this backend. */
- (RYGraphicsBackendType)backendType;

- (BOOL)resizeWithWidth:(unsigned int)width height:(unsigned int)height;
- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight;
- (BOOL)getContentScale:(float *)outScale;
- (BOOL)setContentScale:(float)scale;

/** Returns the backend handle for one of the renderer's built-in pipelines. */
- (void *)builtinPipelineHandle:(RYBuiltinPipeline)pipeline;

- (void *)createTextureWithPixels:(const void *)pixels
                            width:(unsigned int)width
                           height:(unsigned int)height
                            error:(RYError **)error;
- (void)destroyTexture:(void *)textureHandle;

- (void *)createBufferWithData:(const void *)data
                          size:(size_t)size
                          type:(RYBufferType)type
                         error:(RYError **)error;
- (void)destroyBuffer:(void *)bufferHandle;

- (void *)createPipelineWithDesc:(const RYPipelineDesc *)desc
                           error:(RYError **)error;
- (void)destroyPipeline:(void *)pipelineHandle;

- (void *)createRenderTargetWithWidth:(unsigned int)width
                               height:(unsigned int)height
                         colorTexture:(void **)outColorTexture
                         depthTexture:(void **)outDepthTexture
                                error:(RYError **)error;
- (void)destroyRenderTarget:(void *)renderTargetHandle;

- (void *)createStencilWithError:(RYError **)error;
- (void)destroyStencil:(void *)stencilHandle;
- (BOOL)stencilIsEmpty:(void *)stencilHandle;
- (BOOL)clearStencil:(void *)stencilHandle;
- (BOOL)addRectangleToStencil:(void *)stencilHandle
                       matrix:(ryo::Matrix4)matrix
                       camera:(const RYCamera *)camera;
- (BOOL)addRoundedRectangleToStencil:(void *)stencilHandle
                              matrix:(ryo::Matrix4)matrix
                              radius:(float)radius
                              camera:(const RYCamera *)camera;
- (BOOL)addOvalToStencil:(void *)stencilHandle
                  matrix:(ryo::Matrix4)matrix
                  camera:(const RYCamera *)camera;

- (void)frameBegin;
- (BOOL)frameEnd;
- (float)frameTime;

- (void)drawPassBeginWithRenderTarget:(RYRenderTarget *)renderTarget
                           clearColor:(const RYColor *)clearColor;
- (void)drawPassEnd;
- (BOOL)drawEx:(const RYDrawCall *)drawCall;
- (BOOL)drawRectColor:(RYColor)color
               matrix:(ryo::Matrix4)matrix
             settings:(const RYDrawSettings *)settings;
- (BOOL)drawRectTexture:(RYTexture *)texture
            samplerType:(RYSamplerType)samplerType
                  color:(RYColor)color
                 matrix:(ryo::Matrix4)matrix
               settings:(const RYDrawSettings *)settings;
- (BOOL)drawMeshTextureWithVertexBuffer:(RYBuffer *)vertexBuffer
                            indexBuffer:(RYBuffer *)indexBuffer
                             indexCount:(uint32_t)indexCount
                                texture:(RYTexture *)texture
                            samplerType:(RYSamplerType)samplerType
                                  color:(RYColor)color
                                 matrix:(ryo::Matrix4)matrix
                               settings:(const RYDrawSettings *)settings;

@end
