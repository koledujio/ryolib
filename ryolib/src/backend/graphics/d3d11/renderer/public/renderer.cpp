#include "../private.h"

#include <cstdio>

namespace ryo {
    ResourcePool<_InternalRenderer, MAX_RENDERERS> _renderer_pool;

    Renderer renderer_create(const RendererConfig& config) {
        if (_renderer_pool.active_count >= MAX_RENDERERS) {
            error_set("Maximum number of renderers reached");
            return {};
        }

        UINT creationFlags = 0;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG
#endif

        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        D3D_FEATURE_LEVEL achivedFeatureLevel;
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;

        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            1,
            D3D11_SDK_VERSION,
            &device,
            &achivedFeatureLevel,
            &context
        );

        if (FAILED(hr)) {
            error_set("Failed to create D3D11 Device HRESULT: 0x%08X", (unsigned int)hr);
            return {};
        }

        Renderer result = _renderer_create(config, device, context);

        device->Release();
        context->Release();

        return result;
    }

    Renderer renderer_create_share_resources(const RendererConfig& config, Renderer renderer) {
        if (_renderer_pool.active_count >= MAX_RENDERERS) {
            error_set("Maximum number of renderers reached");
            return {};
        }

        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid base renderer handle");

        _InternalRenderer& baseIr = _renderer_pool.items[renderer.index()];
        RYO_ASSERT(baseIr.device != nullptr && baseIr.context != nullptr, {}, "Base renderer has no valid D3D11 device");

        return _renderer_create(config, baseIr.device, baseIr.context);
    }

    void renderer_destroy(Renderer renderer) {
        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");

        INDEX idx = renderer.index();
        _InternalRenderer& ir = _renderer_pool.items[idx];

        _renderer_release_window_targets(ir);
        if (ir.depthStencilState) ir.depthStencilState->Release();
        if (ir.swapChain) ir.swapChain->Release();
        if (ir.context) ir.context->Release();
        if (ir.device) ir.device->Release();

        ir = {};

        _renderer_pool.free_index(idx);
    }

    bool renderer_is_valid(Renderer renderer) {
        INDEX idx = renderer.index();

        if (idx >= MAX_RENDERERS || idx == 0)
            return false;

        return _renderer_pool.is_valid(idx, renderer.gen());
    }

    bool renderer_resize(Renderer renderer, int width, int height) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");

        INDEX idx = renderer.index();
        _InternalRenderer& ir = _renderer_pool.items[idx];
        if (width == ir.width && height == ir.height) {
            error_set("Cannot resize renderer, new size and current size are the same");
            return false;
        }

        // ResizeBuffers requires every view of the old back buffer to be released first.
        ir.context->OMSetRenderTargets(0, nullptr, nullptr);
        _renderer_release_window_targets(ir);

        HRESULT hr = ir.swapChain->ResizeBuffers(
            0,
            width,
            height,
            DXGI_FORMAT_UNKNOWN,
            0
        );

        if (FAILED(hr)) {
            error_set("Failed to resize swap chain HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        if (!_renderer_create_window_targets(ir, width, height)) {
            error_set("Failed to recreate swap-chain render targets after resize");
            return false;
        }

        ir.width = width;
        ir.height = height;

        return true;
    }
}
