#include "../private.h"

bool _renderer_init_pipelines(RYD3D11GraphicsBackend* backend, RYInternalRenderer& internalRenderer) {
    RYPipelineDesc colorDesc = {};
    colorDesc.shader = RYBuiltinShaderColor;
    colorDesc.vertexLayout = RYVertexLayoutTypePosition;
    colorDesc.blendMode = RYBlendModeOpaque;
    void* colorHandle = [backend createPipelineWithDesc:&colorDesc error:nullptr];
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
    void* textureHandle = [backend createPipelineWithDesc:&textureDesc error:nullptr];
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
    void* stencilHandle = [backend createPipelineWithDesc:&stencilDesc error:nullptr];
    if (!stencilHandle) {
        return false;
    }
    internalRenderer.stencilPipeline = [[RYPipeline alloc] initWithInternalPipeline:stencilHandle
                                                                         renderer:nil
                                                                             desc:&stencilDesc];

    return true;
}
