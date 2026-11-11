#include "../../private.h"

@implementation RYD3D11GraphicsBackend (Buffer)

- (void *)createBufferWithData:(const void *)data
                          size:(size_t)size
                          type:(RYBufferType)type
                         error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_internal || size == 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid parameters for RYBuffer"];
        }
        return NULL;
    }

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;

    UINT bindFlags = (type == RYBufferTypeVertex) ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(size);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = bindFlags;

    D3D11_SUBRESOURCE_DATA initData = {};
    D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
    if (data) {
        initData.pSysMem = data;
        initDataPtr = &initData;
    }

    ID3D11Buffer* d3dBuffer = nullptr;
    HRESULT hr = intern->device->CreateBuffer(&desc, initDataPtr, &d3dBuffer);
    if (FAILED(hr)) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsBufferCreationFailed
                                           message:"Failed to create D3D11 buffer"];
        }
        return NULL;
    }

    RYInternalBuffer* buffer = new RYInternalBuffer;
    buffer->buffer = d3dBuffer;
    buffer->type = type;
    buffer->size = size;

    return buffer;
}

- (void)destroyBuffer:(void *)bufferHandle {
    if (!bufferHandle) return;

    RYInternalBuffer* buffer = (RYInternalBuffer*)bufferHandle;
    if (buffer->buffer) buffer->buffer->Release();
    delete buffer;
}

@end
