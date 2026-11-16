#include "../private.h"

namespace ryo {
    bool _pipeline_create_blend_state(ID3D11Device* device, BlendMode blendMode, ID3D11BlendState** result) {
        D3D11_BLEND_DESC blendDesc = {};

        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        switch (blendMode) {
            case BLEND_MODE_OPAQUE:
                blendDesc.RenderTarget[0].BlendEnable = FALSE;
                break;

            case BLEND_MODE_ALPHA:
                blendDesc.RenderTarget[0].BlendEnable = TRUE;
                blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
                blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
                blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
                blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
                blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
                blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
                break;
            
            case BLEND_MODE_ADDITIVE:
                blendDesc.RenderTarget[0].BlendEnable = TRUE;
                blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
                blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
                blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
                blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
                blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
                blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
                break;
        }

        HRESULT hr = device->CreateBlendState(&blendDesc, result);

        if (FAILED(hr)) {
            error_set("Failed to create blend state HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        return true;
    }
}