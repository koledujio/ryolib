#include "../../private.h"

namespace ryo {
    ResourcePool<_InternalTexture, MAX_TEXTURES> _texture_pool;

    Texture texture_create(Renderer renderer, const void* pixels, int width, int height) {
        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");

        RYO_ENSURE(pixels != nullptr, {}, "Pixels pointer is null");
        RYO_ENSURE(width > 0 && height > 0, {}, "Invalid dimensions");

        if (_texture_pool.active_count >= MAX_TEXTURES) {
            error_set("Maximum number of textures reached");
            return {};
        }

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

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
        HRESULT hr = ir.device->CreateTexture2D(&desc, &initData, &tex);
        if (FAILED(hr)) {
            error_set("Failed to create Texture2D HRESULT: 0x%08X", (unsigned int)hr);
            return {};
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        ID3D11ShaderResourceView* srv = nullptr;
        hr = ir.device->CreateShaderResourceView(tex, &srvDesc, &srv);
        if (FAILED(hr)) {
            error_set("Failed to create shader resource view HRESULT: 0x%08X", (unsigned int)hr);
            tex->Release();
            return {};
        }

        INDEX idx = _texture_pool.allocate_index();
        GENID gen = _texture_pool.generations[idx];

        _InternalTexture& it = _texture_pool.items[idx];
        it.texture = tex;
        it.srv = srv;
        it.width = width;
        it.height = height;

        return Texture::pack(idx, gen);
    }

    void texture_destroy(Texture texture) {
        RYO_ENSURE_VOID(texture_is_valid(texture), "Invalid texture handle");

        INDEX idx = texture.index();
        _InternalTexture& it = _texture_pool.items[idx];

        if (it.srv) it.srv->Release();
        if (it.texture) it.texture->Release();

        _texture_pool.free_index(idx);
    }

    bool texture_is_valid(Texture texture) {
        INDEX idx = texture.index();

        if (idx >= MAX_TEXTURES || idx == 0)
            return false;

        return _texture_pool.is_valid(idx, texture.gen());
    }
}