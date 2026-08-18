#include "../private.h"

#include <cstddef>
#include <cstddef>
#include <cstring>

namespace ryo {
    ResourcePool<_InternalPipeline, MAX_PIPELINES> _pipeline_pool;

    Pipeline pipeline_create(Renderer renderer, const PipelineDesc& desc) {
        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");

        if (_pipeline_pool.active_count >= MAX_PIPELINES) {
            error_set("Maximum number of pipelines reached");
            return {};
        }

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        RYO_ASSERT(ir.device != nullptr, {}, "Renderer has no valid ID3D11 device");

        _ShaderSource vertexShader = _pipeline_get_vertex_shader(desc.shader);
        _ShaderSource pixelShader = _pipeline_get_pixel_shader(desc.shader);

        RYO_ASSERT(vertexShader.data != nullptr, {}, "Vertex shader source is null");
        RYO_ASSERT(pixelShader.data != nullptr, {}, "Pixel shader source is null");

        ID3DBlob* vertexBlob = nullptr;
        ID3DBlob* pixelBlob = nullptr;

#if GFX_HLSL_SOURCE_FALLBACK
        vertexBlob = _pipeline_compile_shader(vertexShader);
        pixelBlob = _pipeline_compile_shader(pixelShader);

        if (!vertexBlob || !pixelBlob) {
            if (vertexBlob) vertexBlob->Release();
            if (pixelBlob) pixelBlob->Release();
            return {};
        }
#else
        HRESULT hr = D3DCreateBlob(vertexShader.size, &vertexBlob);
        if (FAILED(hr)) {
            error_set("Failed to create vertex blob HRESULT: 0x%08X", (unsigned int)hr);
            return {};
        }
        memcpy(vertexBlob->GetBufferPointer(), vertexShader.data, vertexShader.size);

        hr = D3DCreateBlob(pixelShader.size, &pixelBlob);
        if (FAILED(hr)) {
            vertexBlob->Release();
            error_set("Failed to create pixel blob HRESULT: 0x%08X", (unsigned int)hr);
            return {};
        }
        memcpy(pixelBlob->GetBufferPointer(), pixelShader.data, pixelShader.size);
#endif

        _InternalPipeline pipeline = {};

        pipeline.vertexLayout = desc.vertexLayout;

        HRESULT hr = ir.device->CreateVertexShader(
            vertexBlob->GetBufferPointer(),
            vertexBlob->GetBufferSize(),
            nullptr,
            &pipeline.vertexShader
        );

        if (FAILED(hr)) {
            error_set("Failed to create vertex shader HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            vertexBlob->Release();
            if (pixelBlob) pixelBlob->Release();
            return {};
        }

        hr = ir.device->CreatePixelShader(
            pixelBlob->GetBufferPointer(),
            pixelBlob->GetBufferSize(),
            nullptr,
            &pipeline.pixelShader
        );

        if (FAILED(hr)) {
            error_set("Failed to create pixel shader HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            vertexBlob->Release();
            pixelBlob->Release();
            _pipeline_release(pipeline);
            return {};
        }

        if(!_pipeline_create_input_layout(desc.vertexLayout, ir, pipeline, vertexBlob, hr)) {
            error_set("Failed to create input layout");
            vertexBlob->Release();
            pixelBlob->Release();
            _pipeline_release(pipeline);
            return {};
        }

        vertexBlob->Release();
        pixelBlob->Release();

        if (FAILED(hr)) {
            error_set("Failed to create input layout HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            _pipeline_release(pipeline);
            return {};
        }

        if (!_pipeline_create_blend_state(ir.device, desc.blendMode, &pipeline.blendState)) {
            _pipeline_release(pipeline);
            return {};
        }

        INDEX idx = _pipeline_pool.allocate_index();
        GENID gen = _pipeline_pool.generations[idx];

        _pipeline_pool.items[idx] = pipeline;

        return Pipeline::pack(idx, gen);
    }

    void pipeline_destroy(Pipeline pipeline) {
        RYO_ENSURE_VOID(pipeline_is_valid(pipeline), "Invalid renderer handle");

        INDEX idx = pipeline.index();

        _pipeline_release(_pipeline_pool.items[idx]);
        _pipeline_pool.free_index(idx);
    }

    bool pipeline_is_valid(Pipeline pipeline) {
        if (pipeline.is_null()) return false;

        INDEX idx = pipeline.index();
        GENID gen = pipeline.gen();

        if (idx >= MAX_PIPELINES || idx == 0)
            return false;

        if (!_pipeline_pool.is_valid(idx, gen)) return false;

        const _InternalPipeline& ip = _pipeline_pool.items[idx];

        return ip.vertexShader != nullptr && ip.pixelShader != nullptr && ip.inputLayout != nullptr;
    }
}