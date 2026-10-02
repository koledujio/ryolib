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
        if (!internal)
            return;

        _renderer_init_pipelines(*this, *internal);

        _create_default_buffers(*this);
    }

    Renderer::Renderer(const RendererConfig& config, const Renderer& renderer) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid base renderer");

        InternalRenderer* intern = _renderer_create(config, renderer.internal->device, renderer.internal->context);

        internal = intern;
        if (!internal)
            return;

        _renderer_init_pipelines(*this, *internal);

        _create_default_buffers(*this);
    }

    Renderer::~Renderer() {
        if (!internal)
            return;

        if (internal->quadVertexBuffer) delete internal->quadVertexBuffer;
        if (internal->quadIndexBuffer) delete internal->quadIndexBuffer;
        if (internal->quadUVVertexBuffer) delete internal->quadUVVertexBuffer;
        if (internal->quadUVIndexBuffer) delete internal->quadUVIndexBuffer;

        if (internal->defaultColorPipeline) delete internal->defaultColorPipeline;
        if (internal->defaultTexturePipeline) delete internal->defaultTexturePipeline;
        if (internal->stencilPipeline) delete internal->stencilPipeline;

        if (internal->vertexCB) internal->vertexCB->Release();
        if (internal->pixelCB) internal->pixelCB->Release();

        if (internal->colorView) internal->colorView->Release();
        if (internal->depthTexture) internal->depthTexture->Release();

        if (internal->stencilWriteState) internal->stencilWriteState->Release();
        for (int i = 0; i < 6; i++) {
            if (i < 3) {
                if (internal->depthStates[i])
                    internal->depthStates[i]->Release();
            } else {
                if (internal->stencilTestStates[i - 3])
                    internal->stencilTestStates[i - 3]->Release();
            }
        }

        constexpr unsigned int samplerTypeCount = 4;
        for (int i = 0; i < samplerTypeCount; i++) {
            if (internal->samplers[i])
                internal->samplers[i]->Release();
        }

        constexpr unsigned int cullingModeCount = 3;
        constexpr unsigned int fillModeCount = 2;
        constexpr unsigned int windingOrderCount = 2;
        for (int c = 0; c < cullingModeCount; c++) {
            for (int f = 0; f < fillModeCount; f++) {
                for (int w = 0; w < windingOrderCount; w++) {
                    if (internal->rasterizers[c][f][w])
                        internal->rasterizers[c][f][w]->Release();
                }
            }
        }

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
            if (internal->quadVertexBuffer) delete internal->quadVertexBuffer;
            if (internal->quadIndexBuffer) delete internal->quadIndexBuffer;
            if (internal->quadUVVertexBuffer) delete internal->quadUVVertexBuffer;
            if (internal->quadUVIndexBuffer) delete internal->quadUVIndexBuffer;

            if (internal->defaultColorPipeline) delete internal->defaultColorPipeline;
            if (internal->defaultTexturePipeline) delete internal->defaultTexturePipeline;
            if (internal->stencilPipeline) delete internal->stencilPipeline;

            if (internal->vertexCB) internal->vertexCB->Release();
            if (internal->pixelCB) internal->pixelCB->Release();

            if (internal->colorView) internal->colorView->Release();
            if (internal->depthTexture) internal->depthTexture->Release();

            if (internal->stencilWriteState) internal->stencilWriteState->Release();
            for (int i = 0; i < 6; i++) {
                if (i < 3) {
                    if (internal->depthStates[i])
                        internal->depthStates[i]->Release();
                } else {
                    if (internal->stencilTestStates[i - 3])
                        internal->stencilTestStates[i - 3]->Release();
                }
            }

            constexpr unsigned int samplerTypeCount = 4;
            for (int i = 0; i < samplerTypeCount; i++) {
                if (internal->samplers[i])
                    internal->samplers[i]->Release();
            }

            constexpr unsigned int cullingModeCount = 3;
            constexpr unsigned int fillModeCount = 2;
            constexpr unsigned int windingOrderCount = 2;
            for (int c = 0; c < cullingModeCount; c++) {
                for (int f = 0; f < fillModeCount; f++) {
                    for (int w = 0; w < windingOrderCount; w++) {
                        if (internal->rasterizers[c][f][w])
                            internal->rasterizers[c][f][w]->Release();
                    }
                }
            }

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
                internal->depthStates[0] != nullptr &&
                internal->depthStates[1] != nullptr &&
                internal->depthStates[2] != nullptr &&
                internal->stencilTestStates[0] != nullptr &&
                internal->stencilTestStates[1] != nullptr &&
                internal->stencilTestStates[2] != nullptr &&
                internal->stencilWriteState != nullptr &&
                internal->width > 0 &&
                internal->height > 0 &&
                internal->vertexCB != nullptr &&
                internal->pixelCB != nullptr
                );
    }

    bool Renderer::size_get(unsigned int& outWidth, unsigned int& outHeight) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        outWidth = static_cast<unsigned int>(internal->width);
        outHeight = static_cast<unsigned int>(internal->height);

        return true;
    }

    bool Renderer::resize(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        // width and height are logical, the framebuffer is rendered at the content scale
        unsigned int pixelWidth = static_cast<unsigned int>(static_cast<float>(width) * internal->contentScale);
        unsigned int pixelHeight = static_cast<unsigned int>(static_cast<float>(height) * internal->contentScale);

        if (pixelWidth == static_cast<unsigned int>(internal->width) &&
            pixelHeight == static_cast<unsigned int>(internal->height)) {
            error_set("Cannot resize renderer, new size and current size are the same");
            return false;
        }

        // ResizeBuffers requires every view of the old back buffer to be released first.
        internal->context->OMSetRenderTargets(0, nullptr, nullptr);
        _renderer_release_window_targets(*internal);

        HRESULT hr = internal->swapChain->ResizeBuffers(
            0,
            pixelWidth,
            pixelHeight,
            DXGI_FORMAT_UNKNOWN,
            0
        );

        if (FAILED(hr)) {
            error_set("Failed to resize swap chain HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        if (!_renderer_create_window_targets(*internal, pixelWidth, pixelHeight)) {
            error_set("Failed to recreate swap-chain render targets after resize");
            return false;
        }

        internal->width = pixelWidth;
        internal->height = pixelHeight;

        return true;
    }
}