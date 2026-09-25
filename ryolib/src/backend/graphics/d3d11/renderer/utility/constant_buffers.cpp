#include "../private.h"

namespace ryo {
    bool _renderer_create_constant_buffers(InternalRenderer& ir) {
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.ByteWidth = 256;

        HRESULT hr = ir.device->CreateBuffer(&cbDesc, nullptr, &ir.vertexCB);
        if (FAILED(hr)) {
            error_set("Failed to create vertex constant buffer 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        hr = ir.device->CreateBuffer(&cbDesc, nullptr, &ir.pixelCB);
        if (FAILED(hr)) {
            error_set("Failed to create pixel constant buffer 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        return true;
    }
}