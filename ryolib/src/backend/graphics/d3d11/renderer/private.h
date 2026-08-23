#include "../private.h"

namespace ryo {
    void _renderer_release_window_targets(InternalRenderer& ir);
    bool _renderer_create_window_targets(InternalRenderer& ir, int width, int height);
    bool _renderer_create_depth_stencil_state(InternalRenderer& ir);
    void _renderer_init_pipelines(const Renderer& renderer, InternalRenderer& internalRenderer);
    InternalRenderer* _renderer_create(const RendererConfig& config, ID3D11Device* device, ID3D11DeviceContext* context);
}