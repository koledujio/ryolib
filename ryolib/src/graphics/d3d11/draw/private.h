#pragma once

#include "../private.h"

ryo::Matrix4 _draw_compute_camera_vp(const RYCamera& camera, float aspect);
void _draw_get_target_size(const RYInternalRenderer& renderer, int& width, int& height);
ID3D11SamplerState* _sampler_get_from_type(RYInternalRenderer& intern, RYSamplerType type);
ID3D11RasterizerState* _get_rasterizer_state(RYInternalRenderer& intern, RYCullMode cull, RYFillMode fill, RYWindingOrder winding);
void _update_constant_buffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const void* data, size_t size);
