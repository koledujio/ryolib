#include "../private.h"

bool _renderer_pipeline_init(RYMetalGraphicsBackend *backend, RYInternalRenderer &internalRenderer) {
    NSError *error = nil;
    id<MTLLibrary> library = nil;

    NSString *bundlePath = [[NSBundle mainBundle] pathForResource:@"shaders" ofType:@"metallib"];
    if (bundlePath) {
        library = [internalRenderer.device newLibraryWithURL:[NSURL fileURLWithPath:bundlePath] error:&error];
    }

    if (!library) {
        NSString *execDir = [[NSBundle mainBundle] resourcePath];
        NSString *fallbackPath = [execDir stringByAppendingPathComponent:@"shaders.metallib"];
        NSURL *fallbackURL = [NSURL fileURLWithPath:fallbackPath];

        library = [internalRenderer.device newLibraryWithURL:fallbackURL error:&error];
    }

    if (!library) {
        return false;
    }

    internalRenderer.shaderLibrary = library;

    RYPipelineDesc colorDesc = {};
    colorDesc.shader = RYBuiltinShaderColor;
    colorDesc.vertexLayout = RYVertexLayoutTypePosition;
    colorDesc.blendMode = RYBlendModeOpaque;
    void *colorHandle = [backend createPipelineWithDesc:&colorDesc error:NULL];
    if (!colorHandle) {
        return false;
    }
    internalRenderer.defaultColorPipeline = [[RYPipeline alloc] initWithInternalPipeline:colorHandle
                                                                               renderer:nil
                                                                                   desc:&colorDesc];

    RYPipelineDesc textureDesc = {};
    textureDesc.shader = RYBuiltinShaderTexture;
    textureDesc.vertexLayout = RYVertexLayoutTypePositionUV;
    textureDesc.blendMode = RYBlendModeOpaque;
    void *textureHandle = [backend createPipelineWithDesc:&textureDesc error:NULL];
    if (!textureHandle) {
        return false;
    }
    internalRenderer.defaultTexturePipeline = [[RYPipeline alloc] initWithInternalPipeline:textureHandle
                                                                                 renderer:nil
                                                                                     desc:&textureDesc];

    RYPipelineDesc stencilDesc = {};
    stencilDesc.shader = RYBuiltinShaderStencil;
    stencilDesc.vertexLayout = RYVertexLayoutTypePosition;
    stencilDesc.blendMode = RYBlendModeOpaque;
    void *stencilHandle = [backend createPipelineWithDesc:&stencilDesc error:NULL];
    if (!stencilHandle) {
        return false;
    }
    internalRenderer.stencilPipeline = [[RYPipeline alloc] initWithInternalPipeline:stencilHandle
                                                                         renderer:nil
                                                                             desc:&stencilDesc];

    return true;
}
