#include "../private.h"

namespace ryo {
    void _renderer_release_window_targets(InternalRenderer& ir) {
        if (ir.colorView) {
            ir.colorView->Release();
            ir.colorView = nullptr;
        }

        if (ir.depthView) {
            ir.depthView->Release();
            ir.depthView = nullptr;
        }

        if (ir.depthTexture) {
            ir.depthTexture->Release();
            ir.depthTexture = nullptr;
        }
    }

    bool _renderer_create_window_targets(InternalRenderer& ir, int width, int height) {
        ID3D11Texture2D* backBuffer = nullptr;

        HRESULT hr = ir.swapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&backBuffer)
        );

        if (FAILED(hr)) {
            error_set("Failed to retrieve swap-chain back buffer HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        hr = ir.device->CreateRenderTargetView(
            backBuffer,
            nullptr,
            &ir.colorView
        );
        backBuffer->Release();

        if (FAILED(hr)) {
            error_set("Failed to create swap-chain view HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        D3D11_TEXTURE2D_DESC depthTextureDesc = {};
        depthTextureDesc.Width = static_cast<UINT>(width);
        depthTextureDesc.Height = static_cast<UINT>(height);
        depthTextureDesc.MipLevels = 1;
        depthTextureDesc.ArraySize = 1;
        depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthTextureDesc.SampleDesc.Count = 1;
        depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
        depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        hr = ir.device->CreateTexture2D(
            &depthTextureDesc,
            nullptr,
            &ir.depthTexture
        );

        if (FAILED(hr)) {
            error_set("Failed to create swap-chain depth texture HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            _renderer_release_window_targets(ir);
            return false;
        }

        hr = ir.device->CreateDepthStencilView(
            ir.depthTexture,
            nullptr,
            &ir.depthView
        );

        if (FAILED(hr)) {
            error_set("Failed to create swap-chain depth-stencil view HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            _renderer_release_window_targets(ir);
            return false;
        }

        return true;
    }
}