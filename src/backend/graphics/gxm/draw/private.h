#include "../private.h"

#include "ryolib/math.h"

namespace ryo {
    void _draw_apply_settings(SceGxmContext* context, const DrawSettings* settings);
    void _draw_apply_sampler_settings(SceGxmTexture* texture, SamplerType samplerType);
    void _draw_apply_depth_settings(SceGxmContext* context, const DrawSettings* settings);
    void _draw_active_target_size_get(const InternalRenderer& renderer, int& outWidth, int& outHeight);
    const Pipeline* _draw_active_pipeline_get(const Pipeline* defaultPipeline, const DrawSettings* settings);
    Matrix4 _draw_compute_mvp(const InternalRenderer& renderer, const Matrix4& modelMatrix, const DrawSettings* settings);
}