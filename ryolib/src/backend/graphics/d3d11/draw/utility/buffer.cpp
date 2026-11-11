#include "../private.h"

namespace ryo {
    bool _draw_create_dynamic_buffer(ID3D11Device* device, const void* data, UINT size, UINT bindFlags, ID3D11Buffer** result) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = size;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = bindFlags;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = device->CreateBuffer(&desc, nullptr, result);

        if (FAILED(hr)) {
            error_set("Failed to create dynamic buffer HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        ID3D11DeviceContext* context = nullptr;
        device->GetImmediateContext(&context);

        D3D11_MAPPED_SUBRESOURCE mapped = {};
        hr = context->Map(*result, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

        if (SUCCEEDED(hr)) {
            memcpy(mapped.pData, data, size);
            context->Unmap(*result, 0);
        }

        context->Release();

        if (FAILED(hr)) {
            (*result)->Release();
            *result = nullptr;
            error_set("Failed to map dynamic buffer HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        return true;
    }
}