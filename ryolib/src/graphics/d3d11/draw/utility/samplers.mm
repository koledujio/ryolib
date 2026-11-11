#include "../private.h"

ID3D11SamplerState* _sampler_get_from_type(RYInternalRenderer& intern, RYSamplerType type) {
    int index = static_cast<int>(type);
    if (index < 0 || index >= 4) index = 0;
    if (intern.samplers[index] != nullptr) {
        return intern.samplers[index];
    }

    D3D11_SAMPLER_DESC desc = {};
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    switch (type) {
        case RYSamplerTypeNearest:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case RYSamplerTypeLinear:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case RYSamplerTypeMinNearestMagLinear:
            desc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case RYSamplerTypeMinLinearMagNearest:
            desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
    }

    intern.device->CreateSamplerState(&desc, &intern.samplers[index]);
    return intern.samplers[index];
}

