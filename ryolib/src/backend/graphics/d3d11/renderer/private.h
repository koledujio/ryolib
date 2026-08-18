#include "../private.h"

namespace ryo {
    void _renderer_release_window_targets(_InternalRenderer& ir);
    bool _renderer_create_window_targets(_InternalRenderer& ir, int width, int height);
    bool _renderer_create_depth_stencil_state(_InternalRenderer& ir);
    void _renderer_init_pipelines(Renderer renderer, _InternalRenderer &internalRenderer);
    Renderer _renderer_create(const RendererConfig& config, ID3D11Device* device, ID3D11DeviceContext* context);
}