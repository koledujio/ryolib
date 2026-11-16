#include "../private.h"

namespace ryo {
    ID3D11RasterizerState* _draw_create_rasterizer(ID3D11Device* device, const DrawSettings* settings) {
        DrawSettings defaults = {};

        CullMode cullMode = settings ? settings->cullMode : defaults.cullMode;
        WindingOrder windingOrder = settings ? settings->windingOrder : defaults.windingOrder;
        FillMode fillMode = settings ? settings->fillMode : defaults.fillMode;

        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = fillMode == FILL_MODE_WIREFRAME ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;

        switch (cullMode) {
            case CULL_MODE_NONE:
                desc.CullMode = D3D11_CULL_NONE;
                break;
            
            case CULL_MODE_FRONT:
                desc.CullMode = D3D11_CULL_FRONT;
                break;
            
            case CULL_MODE_BACK:
                desc.CullMode = D3D11_CULL_BACK;
                break;
        }

        desc.FrontCounterClockwise = windingOrder == WINDING_ORDER_COUNTER_CLOCKWISE;

        desc.DepthClipEnable = TRUE;

        ID3D11RasterizerState* state = nullptr;

        HRESULT hr = device->CreateRasterizerState(&desc, &state);

        if (FAILED(hr)) {
            error_set("Failed to create rasterizer state HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return nullptr;
        }

        return state;
    }
}