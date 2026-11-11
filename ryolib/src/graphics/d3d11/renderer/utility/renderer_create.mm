#include "../private.h"

RYInternalRenderer* _renderer_create(const RYRendererConfig& config, ID3D11Device* device, ID3D11DeviceContext* context) {
    HWND hwnd = static_cast<HWND>(config.nativeWindowHandle);
    if (!hwnd) return nullptr;

    float contentScale = config.contentScale;
    if (contentScale <= 0.0f)
        contentScale = 1.0f;

    // render resolution = logical size * content scale
    int width = static_cast<int>(static_cast<float>(config.width) * contentScale);
    int height = static_cast<int>(static_cast<float>(config.height) * contentScale);

    RYInternalRenderer* intern = new RYInternalRenderer;
    intern->contentScale = contentScale;
    intern->width = width;
    intern->height = height;

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
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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
        intern->context->Release();
        intern->device->Release();
        delete intern;
        return nullptr;
    }

    if (!_renderer_create_window_targets(*intern, width, height) ||
        !_renderer_create_depth_stencil_state(*intern) ||
        !_renderer_create_constant_buffers(*intern)) {
        _renderer_release_window_targets(*intern);
        if (intern->vertexCB) intern->vertexCB->Release();
        if (intern->pixelCB) intern->pixelCB->Release();

        if (intern->stencilWriteState) intern->stencilWriteState->Release();
        for (int i = 0; i < 6; i++) {
            if (i < 3) {
                if (intern->depthStates[i])
                    intern->depthStates[i]->Release();
            } else {
                if (intern->stencilTestStates[i - 3])
                    intern->stencilTestStates[i - 3]->Release();
            }
        }

        if (intern->swapChain) intern->swapChain->Release();
        if (intern->context) intern->context->Release();
        if (intern->device) intern->device->Release();
        delete intern;
        return nullptr;
    }

    return intern;
}
