#include "../private.h"

#include <cstddef>
#include <cstddef>
#include <cstring>

namespace ryo {
    Pipeline::Pipeline(const Renderer& renderer, const PipelineDesc& desc) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");
        RYO_ENSURE_VOID(!renderer.internal->inFrame, "Renderer is in a frame");

        _ShaderSource vertexShader = _pipeline_get_vertex_shader(desc.shader);
        _ShaderSource pixelShader = _pipeline_get_pixel_shader(desc.shader);

        RYO_ASSERT_VOID(vertexShader.data != nullptr, "Vertex shader source is null");
        RYO_ASSERT_VOID(pixelShader.data != nullptr, "Pixel shader source is null");

        ID3DBlob* vertexBlob = nullptr;
        ID3DBlob* pixelBlob = nullptr;

#if GFX_HLSL_SOURCE_FALLBACK
        vertexBlob = _pipeline_compile_shader(vertexShader);
        pixelBlob = _pipeline_compile_shader(pixelShader);

        if (!vertexBlob || !pixelBlob) {
            if (vertexBlob) vertexBlob->Release();
            if (pixelBlob) pixelBlob->Release();
            return;
        }
#else
        HRESULT hr = D3DCreateBlob(vertexShader.size, &vertexBlob);
        if (FAILED(hr)) {
            error_set("Failed to create vertex blob HRESULT: 0x%08X", (unsigned int)hr);
            return;
        }
        memcpy(vertexBlob->GetBufferPointer(), vertexShader.data, vertexShader.size);

        hr = D3DCreateBlob(pixelShader.size, &pixelBlob);
        if (FAILED(hr)) {
            vertexBlob->Release();
            error_set("Failed to create pixel blob HRESULT: 0x%08X", (unsigned int)hr);
            return;
        }
        memcpy(pixelBlob->GetBufferPointer(), pixelShader.data, pixelShader.size);
#endif

        InternalPipeline* intern = new InternalPipeline;

        intern->vertexLayout = desc.vertexLayout;

        HRESULT hr = renderer.internal->device->CreateVertexShader(
            vertexBlob->GetBufferPointer(),
            vertexBlob->GetBufferSize(),
            nullptr,
            &intern->vertexShader
        );

        if (FAILED(hr)) {
            error_set("Failed to create vertex shader HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            vertexBlob->Release();
            if (pixelBlob) pixelBlob->Release();
            delete intern;
            return;
        }

        hr = renderer.internal->device->CreatePixelShader(
            pixelBlob->GetBufferPointer(),
            pixelBlob->GetBufferSize(),
            nullptr,
            &intern->pixelShader
        );

        if (FAILED(hr)) {
            error_set("Failed to create pixel shader HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            vertexBlob->Release();
            pixelBlob->Release();
            _pipeline_release(intern);
            return;
        }

        if(!_pipeline_create_input_layout(desc.vertexLayout, *renderer.internal, *intern, vertexBlob, hr)) {
            error_set("Failed to create input layout");
            vertexBlob->Release();
            pixelBlob->Release();
            _pipeline_release(intern);
            return;
        }

        vertexBlob->Release();
        pixelBlob->Release();

        if (FAILED(hr)) {
            error_set("Failed to create input layout HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            _pipeline_release(intern);
            return;
        }

        if (desc.shader == BUILTIN_SHADER_STENCIL) {
            D3D11_BLEND_DESC blendDesc = {};
            blendDesc.RenderTarget[0].BlendEnable = FALSE;
            blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
            HRESULT blendHr = renderer.internal->device->CreateBlendState(&blendDesc, &intern->blendState);
            if (FAILED(blendHr)) {
                error_set("Failed to create stencil blend state HRESULT: 0x%08X", static_cast<unsigned int>(blendHr));
                _pipeline_release(intern);
                return;
            }
        } else {
            if (!_pipeline_create_blend_state(renderer.internal->device, desc.blendMode, &intern->blendState)) {
                _pipeline_release(intern);
                return;
            }
        }

        internal = intern;
    }

    Pipeline::~Pipeline() {
        if (!internal)
            return;

        _pipeline_release(internal); // checks if exists and releases every thing inside of internal, then deletes internal
    }

    Pipeline::Pipeline(Pipeline&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Pipeline& Pipeline::operator=(Pipeline&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            _pipeline_release(internal);
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Pipeline::valid() const {
        return (internal != nullptr &&
                internal->vertexShader != nullptr &&
                internal->pixelShader != nullptr &&
                internal->inputLayout != nullptr &&
                internal->blendState != nullptr);
    }
}