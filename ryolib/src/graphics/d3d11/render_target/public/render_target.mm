#include "../private.h"

@implementation RYD3D11GraphicsBackend (RenderTarget)

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

    if (!_internal || width == 0 || height == 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid D3D11 backend or dimensions passed to RYRenderTarget"];
        }
        return NULL;
    }

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (intern->inFrame) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsRenderTargetCreationFailed
                                           message:"Cannot create render target while renderer is inside a frame"];
        }
        return NULL;
    }

    RYInternalRenderTarget *target = new RYInternalRenderTarget;
    if (!_render_target_create_textures(intern->device, width, height, *target, outColorTexture, outDepthTexture)) {
        delete target;
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsRenderTargetCreationFailed
                                           message:"Failed to create render target textures/views"];
        }
        return NULL;
    }

    return target;
}

- (void)destroyRenderTarget:(void *)renderTargetHandle {
    if (!renderTargetHandle) return;

    // Color and depth textures are owned by their RYTexture wrappers.
    RYInternalRenderTarget *target = (RYInternalRenderTarget *)renderTargetHandle;
    _render_target_release_views(*target);
    delete target;
}

@end
