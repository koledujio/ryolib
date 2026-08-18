#include "../../private.h"

namespace ryo {
    ResourcePool<_InternalBuffer, MAX_BUFFERS> _buffer_pool;

    Buffer buffer_create(Renderer renderer, const void* data, size_t size, BufferType type) {
        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");
        RYO_ENSURE(size > 0 && data != nullptr, {}, "Invalid size or null data pointer");

        if (_buffer_pool.active_count >= MAX_BUFFERS) {
            error_set("Maximum number of buffers reached");
            return {};
        }

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        UINT bindFlags = 0;
        switch (type) {
            case BUFFER_TYPE_VERTEX:
                bindFlags = D3D11_BIND_VERTEX_BUFFER;
                break;
            case BUFFER_TYPE_INDEX:
                bindFlags = D3D11_BIND_INDEX_BUFFER;
                break;
        }

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = static_cast<UINT>(size);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = bindFlags;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        D3D11_SUBRESOURCE_DATA* pInitData = nullptr;

        if (data != nullptr) {
            initData.pSysMem = data;
            initData.SysMemPitch = 0;
            initData.SysMemSlicePitch = 0;
            pInitData = &initData;
        }

        ID3D11Buffer* d3dBuffer = nullptr;
        HRESULT hr = ir.device->CreateBuffer(&desc, pInitData, &d3dBuffer);
        if (FAILED(hr)) {
            error_set("Failed to create ID3D11Buffer HRESULT: 0x%08X", (unsigned int)hr);
            return {};
        }

        INDEX idx = _buffer_pool.allocate_index();
        GENID gen = _buffer_pool.generations[idx];

        _InternalBuffer& ib = _buffer_pool.items[idx];
        ib.buffer = d3dBuffer;
        ib.type = type;
        ib.size = size;

        return Buffer::pack(idx, gen);
    }

    void buffer_destroy(Buffer buffer) {
        RYO_ENSURE_VOID(buffer_is_valid(buffer), "Invalid buffer handle");

        INDEX idx = buffer.index();
        _InternalBuffer& ib = _buffer_pool.items[idx];

        if (ib.buffer) {
            ib.buffer->Release();
        }

        _buffer_pool.free_index(idx);
    }

    bool buffer_is_valid(Buffer buffer) {
        INDEX idx = buffer.index();

        if (idx >= MAX_BUFFERS || idx == 0)
            return false;

        return _buffer_pool.is_valid(buffer.index(), buffer.gen());
    }
}