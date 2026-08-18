#include "../private.h"

namespace ryo {
    void _renderer_init_pipelines(Renderer renderer, _InternalRenderer &internalRenderer) {
        PipelineDesc colorDesc = {};
        colorDesc.shader = BUILTIN_SHADER_COLOR;
        colorDesc.vertexLayout = VERTEX_LAYOUT_POSITION;
        colorDesc.blendMode = BLEND_MODE_OPAQUE;
        internalRenderer.defaultColorPipeline = pipeline_create(renderer, colorDesc);
        RYO_ASSERT_VOID(pipeline_is_valid(internalRenderer.defaultColorPipeline), "Failed to create default color pipeline");

        PipelineDesc textureDesc = {};
        textureDesc.shader = BUILTIN_SHADER_TEXTURE;
        textureDesc.vertexLayout = VERTEX_LAYOUT_POSITION_UV;
        textureDesc.blendMode = BLEND_MODE_OPAQUE;
        internalRenderer.defaultTexturePipeline = pipeline_create(renderer, textureDesc);
        RYO_ASSERT_VOID(pipeline_is_valid(internalRenderer.defaultTexturePipeline), "Failed to create default texture pipeline");
    }
}