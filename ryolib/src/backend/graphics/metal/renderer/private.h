#pragma once

#include "../private.h"

namespace ryo {
    Renderer _renderer_create(const RendererConfig& config, id<MTLDevice> device);
    bool _renderer_pipeline_init(Renderer renderer, _InternalRenderer& internalRenderer);
    void _renderer_sampler_init(_InternalRenderer& internalRenderer);
}