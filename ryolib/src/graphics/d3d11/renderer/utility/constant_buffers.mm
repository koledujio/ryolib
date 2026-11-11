#include "../private.h"

bool _renderer_create_constant_buffers(RYInternalRenderer& ir) {
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    desc.ByteWidth = sizeof(ryo::Matrix4);
    HRESULT hr = ir.device->CreateBuffer(&desc, nullptr, &ir.vertexCB);
    if (FAILED(hr)) return false;

    desc.ByteWidth = sizeof(RYColor);
    hr = ir.device->CreateBuffer(&desc, nullptr, &ir.pixelCB);
    if (FAILED(hr)) return false;

    return true;
}
