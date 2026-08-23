#include "../private.h"

namespace ryo {
    InternalRenderer* _renderer_create(const RendererConfig& config, ID3D11Device* device, ID3D11DeviceContext* context) {
        HWND hwnd = static_cast<HWND>(config.nativeWindowHandle);
        RYO_ASSERT(hwnd, nullptr, "Failed to get native window handle HWND");

        InternalRenderer* intern = new InternalRenderer;
        intern->width = config.width;
        intern->height = config.height;

        intern->device = device;
        intern->device->AddRef();

        intern->context = context;
        intern->context->AddRef();

        IDXGIDevice* dxgiDevice = nullptr;
        device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

        IDXGIAdapter* dxgiAdapter = nullptr;
        dxgiDevice->GetAdapter(&dxgiAdapter);

        IDXGIFactory* dxgiFactory = nullptr;
        dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

        DXGI_SWAP_CHAIN_DESC scd = {};
        scd.BufferCount = 1;
        scd.BufferDesc.Width = config.width;
        scd.BufferDesc.Height = config.height;
        scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // same as metal
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = hwnd;
        scd.SampleDesc.Count = 1;
        scd.SampleDesc.Quality = 0;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        HRESULT hr = dxgiFactory->CreateSwapChain(device, &scd, &intern->swapChain);

        dxgiFactory->Release();
        dxgiAdapter->Release();
        dxgiDevice->Release();

        if (FAILED(hr)) {
            error_set("Failed to create swap chain HRESULT: 0x%08X", (unsigned int)hr);
            intern->context->Release();
            intern->device->Release();
            delete intern;
            return nullptr;
        }

        if (!_renderer_create_window_targets(*intern, config.width, config.height) ||
            !_renderer_create_depth_stencil_state(*intern)) {
            _renderer_release_window_targets(*intern);
            if (intern->depthState) intern->depthState->Release();
            if (intern->swapChain) intern->swapChain->Release();
            if (intern->context) intern->context->Release();
            if (intern->device) intern->device->Release();
            delete intern;
            return nullptr;
        }

        return intern;
    }
}