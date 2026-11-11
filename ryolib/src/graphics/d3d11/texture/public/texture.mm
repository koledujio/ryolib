#include "../../private.h"

@implementation RYD3D11GraphicsBackend (Texture)

- (void *)createTextureWithPixels:(const void *)pixels
                            width:(unsigned int)width
                           height:(unsigned int)height
                            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_internal || width == 0 || height == 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid parameters for RYTexture"];
        }
        return NULL;
    }

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
    if (pixels) {
        initData.pSysMem = pixels;
        initData.SysMemPitch = static_cast<UINT>(width * 4);
        initDataPtr = &initData;
    }

    ID3D11Texture2D* tex = nullptr;
    HRESULT hr = intern->device->CreateTexture2D(&desc, initDataPtr, &tex);
    if (FAILED(hr)) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsTextureCreationFailed
                                           message:"Failed to create D3D11 texture"];
        }
        return NULL;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    ID3D11ShaderResourceView* srv = nullptr;
    hr = intern->device->CreateShaderResourceView(tex, &srvDesc, &srv);
    if (FAILED(hr)) {
        tex->Release();
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsTextureCreationFailed
                                           message:"Failed to create shader resource view"];
        }
        return NULL;
    }

    RYInternalTexture* texture = new RYInternalTexture;
    texture->texture = tex;
    texture->srv = srv;
    texture->width = width;
    texture->height = height;

    return texture;
}

- (void)destroyTexture:(void *)textureHandle {
    if (!textureHandle) return;

    RYInternalTexture* texture = (RYInternalTexture*)textureHandle;
    if (texture->srv) texture->srv->Release();
    if (texture->texture) texture->texture->Release();
    delete texture;
}

@end
