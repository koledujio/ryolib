#pragma once

#include "../private.h"

RYInternalRenderer* _renderer_create(const RYRendererConfig &config, id<MTLDevice> device, RYError **error);
bool _renderer_pipeline_init(RYMetalGraphicsBackend *backend, RYInternalRenderer &internalRenderer);
void _renderer_sampler_init(RYInternalRenderer &internalRenderer);
bool _renderer_buffers_init(RYMetalGraphicsBackend *backend, RYInternalRenderer &internalRenderer);
