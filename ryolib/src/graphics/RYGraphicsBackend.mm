#import "ryolib/graphics/RYGraphicsBackend.h"

static void _set_not_supported_error(RYError **error, const char *message) {
    if (error) {
        *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsFeatureNotSupported message:message];
    }
}

@implementation RYGraphicsBackend

- (instancetype)initBackendWithConfig:(const RYRendererConfig *)config
                         shareBackend:(RYGraphicsBackend *)shareBackend
                                error:(RYError **)error {
    self = [super init];
    if (!self) {
        return nil;
    }

    _set_not_supported_error(error, "Graphics backend does not support initialization");
    [self release];
    return nil;
}

- (RYGraphicsBackendType)backendType {
    return RYGraphicsBackendCustom;
}

- (BOOL)resizeWithWidth:(unsigned int)width height:(unsigned int)height {
    return NO;
}

- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight {
    return NO;
}

- (BOOL)getContentScale:(float *)outScale {
    return NO;
}

- (BOOL)setContentScale:(float)scale {
    return NO;
}

- (void *)builtinPipelineHandle:(RYBuiltinPipeline)pipeline {
    return NULL;
}

- (void *)createTextureWithPixels:(const void *)pixels
                            width:(unsigned int)width
                           height:(unsigned int)height
                            error:(RYError **)error {
    _set_not_supported_error(error, "Graphics backend does not support creating textures");
    return NULL;
}

- (void)destroyTexture:(void *)textureHandle {
}

- (void *)createBufferWithData:(const void *)data
                          size:(size_t)size
                          type:(RYBufferType)type
                         error:(RYError **)error {
    _set_not_supported_error(error, "Graphics backend does not support creating buffers");
    return NULL;
}

- (void)destroyBuffer:(void *)bufferHandle {
}

- (void *)createPipelineWithDesc:(const RYPipelineDesc *)desc
                           error:(RYError **)error {
    _set_not_supported_error(error, "Graphics backend does not support creating pipelines");
    return NULL;
}

- (void)destroyPipeline:(void *)pipelineHandle {
}

- (void *)createRenderTargetWithWidth:(unsigned int)width
                               height:(unsigned int)height
                         colorTexture:(void **)outColorTexture
                         depthTexture:(void **)outDepthTexture
                                error:(RYError **)error {
    _set_not_supported_error(error, "Graphics backend does not support creating render targets");
    return NULL;
}

- (void)destroyRenderTarget:(void *)renderTargetHandle {
}

- (void *)createStencilWithError:(RYError **)error {
    _set_not_supported_error(error, "Graphics backend does not support creating stencils");
    return NULL;
}

- (void)destroyStencil:(void *)stencilHandle {
}

- (BOOL)stencilIsEmpty:(void *)stencilHandle {
    return YES;
}

- (BOOL)clearStencil:(void *)stencilHandle {
    return NO;
}

- (BOOL)addRectangleToStencil:(void *)stencilHandle
                       matrix:(ryo::Matrix4)matrix
                       camera:(const RYCamera *)camera {
    return NO;
}

- (BOOL)addRoundedRectangleToStencil:(void *)stencilHandle
                              matrix:(ryo::Matrix4)matrix
                              radius:(float)radius
                              camera:(const RYCamera *)camera {
    return NO;
}

- (BOOL)addOvalToStencil:(void *)stencilHandle
                  matrix:(ryo::Matrix4)matrix
                  camera:(const RYCamera *)camera {
    return NO;
}

- (void)frameBegin {
}

- (BOOL)frameEnd {
    return NO;
}

- (float)frameTime {
    return 0.0f;
}

- (void)drawPassBeginWithRenderTarget:(RYRenderTarget *)renderTarget
                           clearColor:(const RYColor *)clearColor {
}

- (void)drawPassEnd {
}

- (BOOL)drawEx:(const RYDrawCall *)drawCall {
    return NO;
}

- (BOOL)drawRectColor:(RYColor)color
               matrix:(ryo::Matrix4)matrix
             settings:(const RYDrawSettings *)settings {
    return NO;
}

- (BOOL)drawRectTexture:(RYTexture *)texture
            samplerType:(RYSamplerType)samplerType
                  color:(RYColor)color
                 matrix:(ryo::Matrix4)matrix
               settings:(const RYDrawSettings *)settings {
    return NO;
}

- (BOOL)drawMeshTextureWithVertexBuffer:(RYBuffer *)vertexBuffer
                            indexBuffer:(RYBuffer *)indexBuffer
                             indexCount:(uint32_t)indexCount
                                texture:(RYTexture *)texture
                            samplerType:(RYSamplerType)samplerType
                                  color:(RYColor)color
                                 matrix:(ryo::Matrix4)matrix
                               settings:(const RYDrawSettings *)settings {
    return NO;
}

@end
