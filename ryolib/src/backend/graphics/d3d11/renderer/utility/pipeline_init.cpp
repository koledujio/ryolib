#include "../private.h"

namespace ryo {
    void _renderer_init_pipelines(const Renderer& renderer, InternalRenderer &internalRenderer) {
        PipelineDesc colorDesc = {};
        colorDesc.shader = BUILTIN_SHADER_COLOR;
        colorDesc.vertexLayout = VERTEX_LAYOUT_POSITION;
        colorDesc.blendMode = BLEND_MODE_OPAQUE;
        internalRenderer.defaultColorPipeline = new Pipeline(renderer, colorDesc);
        RYO_ASSERT_VOID(internalRenderer.defaultColorPipeline->valid(), "Failed to create default color pipeline");

        PipelineDesc textureDesc = {};
        textureDesc.shader = BUILTIN_SHADER_TEXTURE;
        textureDesc.vertexLayout = VERTEX_LAYOUT_POSITION_UV;
        textureDesc.blendMode = BLEND_MODE_OPAQUE;
        internalRenderer.defaultTexturePipeline = new Pipeline(renderer, textureDesc);
        RYO_ASSERT_VOID(internalRenderer.defaultTexturePipeline->valid(), "Failed to create default texture pipeline");
    }
}