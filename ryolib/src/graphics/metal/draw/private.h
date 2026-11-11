#pragma once

#include "../private.h"

ryo::Matrix4 _compute_camera_vp(const RYCamera &camera, float aspect);
id<MTLSamplerState> _sampler_get_from_type(RYInternalRenderer &internalRenderer, RYSamplerType type);
id<MTLTexture> _draw_get_color_texture(RYInternalRenderer &ir, RYRenderTarget *target);
id<MTLTexture> _draw_get_depth_texture(RYInternalRenderer &ir, RYRenderTarget *target);
void _draw_get_target_size(const RYInternalRenderer &renderer, int &width, int &height);
