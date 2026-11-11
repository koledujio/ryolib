#include "../../private.h"

namespace ryo {
    Buffer::Buffer(const Renderer& renderer, const void* data, size_t size, BufferType type) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");
        RYO_ENSURE_VOID(size > 0 && data != nullptr, "Invalid size or null data pointer");
        RYO_ENSURE_VOID(!renderer.internal->inFrame, "Renderer is in a frame");

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
        initData.pSysMem = data;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;
        pInitData = &initData;

        ID3D11Buffer* d3dBuffer = nullptr;
        HRESULT hr = renderer.internal->device->CreateBuffer(&desc, pInitData, &d3dBuffer);
        if (FAILED(hr)) {
            error_set("Failed to create ID3D11Buffer HRESULT: 0x%08X", (unsigned int)hr);
            return;
        }

        InternalBuffer* intern = new InternalBuffer;
        intern->buffer = d3dBuffer;
        intern->type = type;
        intern->size = size;

        internal = intern;
    }

    Buffer::~Buffer() {
        if (!internal)
            return;

        if (internal->buffer)
            internal->buffer->Release();

        delete internal;
    }

    Buffer::Buffer(Buffer&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Buffer& Buffer::operator=(Buffer&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->buffer)
                internal->buffer->Release();

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Buffer::valid() const {
        return internal != nullptr && internal->buffer != nullptr;
    }
}