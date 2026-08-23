#include "../../private.h"

namespace ryo {
    Texture::Texture(const Renderer& renderer, const void* pixels, unsigned int width, unsigned int height) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");
        RYO_ENSURE_VOID(!renderer.internal->inFrame, "Renderer is in a frame");
        RYO_ENSURE_VOID(pixels != nullptr, "Pixels pointer is null");
        RYO_ENSURE_VOID(width > 0 && height > 0, "Invalid dimensions");

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = pixels;
        initData.SysMemPitch = static_cast<UINT>(width * 4); // 4 bytes per pixel
        initData.SysMemSlicePitch = 0;

        ID3D11Texture2D* tex = nullptr;
        HRESULT hr = renderer.internal->device->CreateTexture2D(&desc, &initData, &tex);
        if (FAILED(hr)) {
            error_set("Failed to create Texture2D HRESULT: 0x%08X", (unsigned int)hr);
            return;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        ID3D11ShaderResourceView* srv = nullptr;
        hr = renderer.internal->device->CreateShaderResourceView(tex, &srvDesc, &srv);
        if (FAILED(hr)) {
            error_set("Failed to create shader resource view HRESULT: 0x%08X", (unsigned int)hr);
            tex->Release();
            return;
        }

        InternalTexture* intern = new InternalTexture;
        intern->texture = tex;
        intern->srv = srv;
        intern->width = width;
        intern->height = height;

        internal = intern;
    }

    Texture::Texture(InternalTexture* intern) {
        internal = intern;
    }

    Texture::~Texture() {
        if (!internal)
            return;

        if (internal->srv) internal->srv->Release();
        if (internal->texture) internal->texture->Release();
        
        delete internal;
    }

    Texture::Texture(Texture&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Texture& Texture::operator=(Texture&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->srv)
                internal->srv->Release();

            if (internal->texture)
                internal->texture->Release();

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Texture::valid() const {
        return internal != nullptr && internal->srv != nullptr && internal->texture != nullptr;
    }
}