#pragma once

#include "../private.h"

void _renderer_release_window_targets(RYInternalRenderer& ir);
bool _renderer_create_window_targets(RYInternalRenderer& ir, int width, int height);
bool _renderer_create_depth_stencil_state(RYInternalRenderer& ir);
bool _renderer_init_pipelines(RYD3D11GraphicsBackend* backend, RYInternalRenderer& internalRenderer);
RYInternalRenderer* _renderer_create(const RYRendererConfig& config, ID3D11Device* device, ID3D11DeviceContext* context);
bool _create_default_buffers(RYD3D11GraphicsBackend* backend, RYInternalRenderer& internalRenderer);
bool _renderer_create_constant_buffers(RYInternalRenderer& ir);
