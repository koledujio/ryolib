#include "../private.h"

namespace ryo {
    bool _render_target_create_textures(ID3D11Device* device, int width, int height, _InternalRenderTarget& result) {
        if (_texture_pool.active_count + 1 >= MAX_TEXTURES) {
            error_set("Not enough texture slots for render target(texture limit reached)");
            return false;
        }

        ID3D11Texture2D* colorTexture = nullptr;
        ID3D11ShaderResourceView* colorSrv = nullptr;
        ID3D11RenderTargetView* colorView = nullptr;

        ID3D11Texture2D* depthTexture = nullptr;
        ID3D11ShaderResourceView* depthSrv = nullptr;
        ID3D11DepthStencilView* depthView = nullptr;

        HRESULT hr;

        // color

        D3D11_TEXTURE2D_DESC colorDesc = {};
        colorDesc.Width = static_cast<UINT>(width);
        colorDesc.Height = static_cast<UINT>(height);
        colorDesc.MipLevels = 1;
        colorDesc.ArraySize = 1;
        colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorDesc.SampleDesc.Count = 1;
        colorDesc.Usage = D3D11_USAGE_DEFAULT;
        colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        hr = device->CreateTexture2D(&colorDesc, nullptr, &colorTexture);

        if (FAILED(hr)) {
            error_set("Failed to create color texture HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        D3D11_RENDER_TARGET_VIEW_DESC colorViewDesc = {};
        colorViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        colorViewDesc.Texture2D.MipSlice = 0;

        hr = device->CreateRenderTargetView(colorTexture, &colorViewDesc, &colorView);

        if (FAILED(hr)) {
            error_set("Failed to create color view HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            colorTexture->Release();
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC colorSrvDesc = {};
        colorSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        colorSrvDesc.Texture2D.MostDetailedMip = 0;
        colorSrvDesc.Texture2D.MipLevels = 1;

        hr = device->CreateShaderResourceView(colorTexture, &colorSrvDesc, &colorSrv);

        if (FAILED(hr)) {
            error_set("Failed to create color shader resource view HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            colorTexture->Release();
            colorView->Release();
            return false;
        }

        // depth

        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = static_cast<UINT>(width);
        depthDesc.Height = static_cast<UINT>(height);
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

        hr = device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);

        if (FAILED(hr)) {
            error_set("Failed to create depth texture HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            colorSrv->Release();
            colorView->Release();
            colorTexture->Release();
            return false;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
        depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthViewDesc.Texture2D.MipSlice = 0;

        hr = device->CreateDepthStencilView(depthTexture, &depthViewDesc, &depthView);

        if (FAILED(hr)) {
            error_set("Failed to create depth view HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            depthTexture->Release();
            colorSrv->Release();
            colorView->Release();
            colorTexture->Release();
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
        depthSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        depthSrvDesc.Texture2D.MostDetailedMip = 0;
        depthSrvDesc.Texture2D.MipLevels = 1;

        hr = device->CreateShaderResourceView(depthTexture, &depthSrvDesc, &depthSrv);

        if (FAILED(hr)) {
            error_set("Failed to create depth shader resource view HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            depthView->Release();
            depthTexture->Release();
            colorSrv->Release();
            colorView->Release();
            colorTexture->Release();
            return false;
        }

        // other

        INDEX colorIdx = _texture_pool.allocate_index();
        GENID colorGen = _texture_pool.generations[colorIdx];

        _InternalTexture&  colorIt = _texture_pool.items[colorIdx];
        colorIt.texture = colorTexture;
        colorIt.srv = colorSrv;
        colorIt.width = width;
        colorIt.height = height;

        INDEX depthIdx = _texture_pool.allocate_index();
        GENID depthGen = _texture_pool.generations[depthIdx];

        _InternalTexture& depthIt = _texture_pool.items[depthIdx];
        depthIt.texture = depthTexture;
        depthIt.srv = depthSrv;
        depthIt.width = width;
        depthIt.height = height;

        result.colorTexture = colorTexture;
        result.colorView = colorView;
        result.colorTextureHandle = Texture::pack(colorIdx, colorGen);
        result.depthTexture = depthTexture;
        result.depthView = depthView;
        result.depthTextureHandle = Texture::pack(depthIdx, depthGen);
        result.width = width;
        result.height = height;

        return true;
    }
}