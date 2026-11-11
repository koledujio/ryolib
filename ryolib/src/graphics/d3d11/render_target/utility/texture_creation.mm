#include "../private.h"

bool _render_target_create_textures(ID3D11Device* device, int width, int height, RYInternalRenderTarget& result, void** outColorTexture, void** outDepthTexture) {
    if (!outColorTexture || !outDepthTexture) {
        return false;
    }

    *outColorTexture = nullptr;
    *outDepthTexture = nullptr;

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
    if (FAILED(hr)) return false;

    D3D11_RENDER_TARGET_VIEW_DESC colorViewDesc = {};
    colorViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    colorViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    colorViewDesc.Texture2D.MipSlice = 0;

    hr = device->CreateRenderTargetView(colorTexture, &colorViewDesc, &colorView);
    if (FAILED(hr)) {
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
        depthView->Release();
        depthTexture->Release();
        colorSrv->Release();
        colorView->Release();
        colorTexture->Release();
        return false;
    }

    RYInternalTexture* colorIntern = new RYInternalTexture;
    colorIntern->texture = colorTexture;
    colorIntern->srv = colorSrv;
    colorIntern->width = width;
    colorIntern->height = height;

    RYInternalTexture* depthIntern = new RYInternalTexture;
    depthIntern->texture = depthTexture;
    depthIntern->srv = depthSrv;
    depthIntern->width = width;
    depthIntern->height = height;

    result.colorView = colorView;
    result.depthView = depthView;
    result.width = width;
    result.height = height;

    *outColorTexture = colorIntern;
    *outDepthTexture = depthIntern;

    return true;
}
