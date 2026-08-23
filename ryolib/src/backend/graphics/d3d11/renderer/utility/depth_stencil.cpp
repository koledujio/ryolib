#include "../private.h"

namespace ryo {
    bool _renderer_create_depth_stencil_state(InternalRenderer& ir) {
        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = FALSE;

        HRESULT hr = ir.device->CreateDepthStencilState(
            &desc,
            &ir.depthState
        );

        if (FAILED(hr)) {
            error_set("Failed to create depth-stencil state HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        return true;
    }
}