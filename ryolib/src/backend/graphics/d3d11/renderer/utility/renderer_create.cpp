#include "../private.h"

namespace ryo {
    Renderer _renderer_create(const RendererConfig& config, ID3D11Device* device, ID3D11DeviceContext* context) {
        HWND hwnd = static_cast<HWND>(config.nativeWindowHandle);
        RYO_ASSERT(hwnd, {}, "Failed to get native window handle HWND");

        INDEX idx = _renderer_pool.allocate_index();
        GENID gen = _renderer_pool.generations[idx];
        _InternalRenderer& ir = _renderer_pool.items[idx];
        ir.width = config.width;
        ir.height = config.height;

        ir.device = device;
        ir.device->AddRef();

        ir.context = context;
        ir.context->AddRef();

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

        HRESULT hr = dxgiFactory->CreateSwapChain(device, &scd, &ir.swapChain);

        dxgiFactory->Release();
        dxgiAdapter->Release();
        dxgiDevice->Release();

        if (FAILED(hr)) {
            error_set("Failed to create swap chain HRESULT: 0x%08X", (unsigned int)hr);
            _renderer_pool.free_index(idx);
            return {};
        }

        if (!_renderer_create_window_targets(ir, config.width, config.height) ||
            !_renderer_create_depth_stencil_state(ir)) {
            _renderer_release_window_targets(ir);
            if (ir.depthStencilState) ir.depthStencilState->Release();
            if (ir.swapChain) ir.swapChain->Release();
            if (ir.context) ir.context->Release();
            if (ir.device) ir.device->Release();
            ir = {};
            _renderer_pool.free_index(idx);
            return {};
        }

        _renderer_init_pipelines(Renderer::pack(idx, gen), ir);

        return Renderer::pack(idx, gen);
    }
}