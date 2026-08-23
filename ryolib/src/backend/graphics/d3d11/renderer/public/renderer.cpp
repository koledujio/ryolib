#include "../private.h"

#include <cstdio>

namespace ryo {
    Renderer::Renderer(const RendererConfig& config) {
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
            return;
        }

        InternalRenderer* intern = _renderer_create(config, device, context);
        
        device->Release();
        context->Release();

        internal = intern;

        _renderer_init_pipelines(*this, *internal);
    }

    Renderer::Renderer(const RendererConfig& config, const Renderer& renderer) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid base renderer");

        InternalRenderer* intern = _renderer_create(config, renderer.internal->device, renderer.internal->context);

        internal = intern;

        _renderer_init_pipelines(*this, *internal);
    }

    Renderer::~Renderer() {
        if (!internal)
            return;

        if (internal->defaultColorPipeline) delete internal->defaultColorPipeline;
        if (internal->defaultTexturePipeline) delete internal->defaultTexturePipeline;

        if (internal->colorView) internal->colorView->Release();
        if (internal->depthTexture) internal->depthTexture->Release();
        if (internal->depthState) internal->depthState->Release();

        if (internal->swapChain) internal->swapChain->Release();
        if (internal->context) internal->context->Release();
        if (internal->device) internal->device->Release();

        delete internal;
    }

    Renderer::Renderer(Renderer&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Renderer& Renderer::operator=(Renderer&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->defaultColorPipeline) delete internal->defaultColorPipeline;
            if (internal->defaultTexturePipeline) delete internal->defaultTexturePipeline;

            if (internal->colorView) internal->colorView->Release();
            if (internal->depthTexture) internal->depthTexture->Release();
            if (internal->depthState) internal->depthState->Release();

            if (internal->swapChain) internal->swapChain->Release();
            if (internal->context) internal->context->Release();
            if (internal->device) internal->device->Release();

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Renderer::valid() const {
        return (internal != nullptr &&
                internal->device != nullptr &&
                internal->context != nullptr &&
                internal->swapChain != nullptr &&
                internal->colorView != nullptr &&
                internal->depthTexture != nullptr &&
                internal->depthView != nullptr &&
                internal->depthState != nullptr &&
                internal->width > 0 &&
                internal->height > 0);
    }

    bool Renderer::resize(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        if (width == internal->width && height == internal->height) {
            error_set("Cannot resize renderer, new size and current size are the same");
            return false;
        }

        // ResizeBuffers requires every view of the old back buffer to be released first.
        internal->context->OMSetRenderTargets(0, nullptr, nullptr);
        _renderer_release_window_targets(*internal);

        HRESULT hr = internal->swapChain->ResizeBuffers(
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

        if (!_renderer_create_window_targets(*internal, width, height)) {
            error_set("Failed to recreate swap-chain render targets after resize");
            return false;
        }

        internal->width = width;
        internal->height = height;

        return true;
    }
}