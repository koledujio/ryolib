#include "../private.h"

@implementation RYMetalGraphicsBackend (RenderTarget)

- (void *)createRenderTargetWithWidth:(unsigned int)width
                               height:(unsigned int)height
                         colorTexture:(void **)outColorTexture
                         depthTexture:(void **)outDepthTexture
                                error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (outColorTexture) *outColorTexture = NULL;
    if (outDepthTexture) *outDepthTexture = NULL;

    if (!_internal) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                           message:"Metal backend is not initialized"];
        }
        return NULL;
    }

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;

    id<MTLTexture> mtlColorTexture = _render_target_create_color_texture(intern->device, width, height);
    id<MTLTexture> mtlDepthTexture = _render_target_create_depth_texture(intern->device, width, height);

    if (!mtlColorTexture || !mtlDepthTexture) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsRenderTargetCreationFailed
                                           message:"Failed to create render target textures"];
        }
        if (mtlColorTexture) [mtlColorTexture release];
        if (mtlDepthTexture) [mtlDepthTexture release];
        return NULL;
    }

    RYInternalTexture *colorTexture = new RYInternalTexture;
    colorTexture->texture = mtlColorTexture;

    RYInternalTexture *depthTexture = new RYInternalTexture;
    depthTexture->texture = mtlDepthTexture;

    RYInternalRenderTarget *target = new RYInternalRenderTarget;
    target->width = width;
    target->height = height;

    if (outColorTexture) *outColorTexture = colorTexture;
    if (outDepthTexture) *outDepthTexture = depthTexture;

    return target;
}

- (void)destroyRenderTarget:(void *)renderTargetHandle {
    if (!renderTargetHandle) return;

    // Color and depth textures are owned by their RYTexture wrappers.
    RYInternalRenderTarget *target = (RYInternalRenderTarget *)renderTargetHandle;
    delete target;
}

@end
