#include "../private.h"

namespace ryo {
    ID3D11SamplerState* _draw_create_sampler(ID3D11Device* device, SamplerType samplerType) {
        D3D11_SAMPLER_DESC desc = {};
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        switch (samplerType) {
            case SAMPLER_TYPE_NEAREST:
                desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                break;
            
            case SAMPLER_TYPE_LINEAR:
                desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                break;
            
            case SAMPLER_TYPE_MIN_NEAREST_MAG_LINEAR:
                desc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
                break;
            
            case SAMPLER_TYPE_MIN_LINEAR_MAG_NEAREST:
                desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; // no D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_POINT in scope for some reason
                break;
        }

        ID3D11SamplerState* sampler = nullptr;

        HRESULT hr = device->CreateSamplerState(&desc, &sampler);

        if (FAILED(hr)) {
            error_set("Failed to create sampler state HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return nullptr;
        }

        return sampler;
    }
}