#include "../private.h"

namespace ryo {
    ID3D11SamplerState* _draw_create_sampler(ID3D11Device* device, SamplerType samplerType);
    ID3D11RasterizerState* _draw_create_rasterizer(ID3D11Device* device, const DrawSettings* settings);
    bool _draw_bind_pipeline(_InternalRenderer& renderer, Pipeline pipeline, const DrawSettings* settings);
    Pipeline _draw_select_pipeline(const DrawSettings* settings, Pipeline defaultPipeline);
    Matrix4 _draw_compute_camera_vp(const Camera& camera, float aspect);
    Matrix4 _draw_get_mvp(const _InternalRenderer& renderer, const DrawSettings* settings, const Matrix4& matrix, int targetWidth, int targetHeight);
    bool _draw_bind_mvp(_InternalRenderer& renderer, const Matrix4& mvp);
    bool _draw_bind_color(_InternalRenderer& renderer, const Color& color);
    bool _draw_create_dynamic_buffer(ID3D11Device* device, const void* data, UINT size, UINT bindFlags, ID3D11Buffer** result);
    void _draw_get_target_size(const _InternalRenderer& renderer, int& width, int& height);
}