#pragma once

#include "../private.h"

namespace ryo {
    bool _renderer_create(Renderer& renderer, const RendererConfig& config, id<MTLDevice> device);
    bool _renderer_pipeline_init(Renderer& renderer, InternalRenderer& internalRenderer);
    void _renderer_sampler_init(InternalRenderer& internalRenderer);
    bool _renderer_buffers_init(Renderer& renderer, InternalRenderer& internalRenderer);
}