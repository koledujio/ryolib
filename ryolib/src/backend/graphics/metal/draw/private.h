#include "../private.h"

namespace ryo {
    Matrix4 _compute_camera_vp(const Camera& camera, float aspect);
    void _draw_apply_settings(id<MTLRenderCommandEncoder> encoder, const DrawSettings* settings);
    id<MTLSamplerState> _sampler_get_from_type(_InternalRenderer& internalRenderer, SamplerType type);
    id<MTLTexture> _draw_get_color_texture(_InternalRenderer& ir, RenderTarget* target);
    id<MTLTexture> _draw_get_depth_texture(_InternalRenderer& ir, RenderTarget* target);
    id<MTLRenderPipelineState> _draw_get_pso(const DrawSettings* settings, const Pipeline& defaultPipeline);
    void _draw_get_target_size(const _InternalRenderer& renderer, int& width, int& height);
}