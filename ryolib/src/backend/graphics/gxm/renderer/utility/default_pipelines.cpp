#include "../private.h"

namespace ryo {
    bool _create_default_pipelines(InternalRenderer* intern, Renderer& renderer) {
        bool fail = false;

        PipelineDesc defaultColorPipelineDesc = {};
        defaultColorPipelineDesc.shader = BUILTIN_SHADER_COLOR;
        defaultColorPipelineDesc.vertexLayout = VERTEX_LAYOUT_POSITION;
        defaultColorPipelineDesc.blendMode = BLEND_MODE_OPAQUE;
        intern->defaultColorPipeline = new Pipeline(renderer, defaultColorPipelineDesc);
        if (!intern->defaultColorPipeline->valid()) {
            RYO_ASSERT(false, false, "Failed to create default color pipeline");
            delete intern->defaultColorPipeline;
            intern->defaultColorPipeline = nullptr;
            fail = true;
        }

        PipelineDesc defaultTexturePipelineDesc = {};
        defaultTexturePipelineDesc.shader = BUILTIN_SHADER_TEXTURE;
        defaultTexturePipelineDesc.vertexLayout = VERTEX_LAYOUT_POSITION_UV;
        defaultTexturePipelineDesc.blendMode = BLEND_MODE_OPAQUE;
        intern->defaultTexturePipeline = new Pipeline(renderer, defaultTexturePipelineDesc);
        if (!intern->defaultTexturePipeline->valid()) {
            RYO_ASSERT(false, false, "Failed to create default texture pipeline");
            delete intern->defaultTexturePipeline;
            intern->defaultTexturePipeline = nullptr;
            fail = true;
        }

        return !fail;
    }
}