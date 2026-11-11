#include "../private.h"

@implementation RYD3D11GraphicsBackend (Pipeline)

- (void *)createPipelineWithDesc:(const RYPipelineDesc *)desc
                           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_internal || !desc) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid D3D11 backend or pipeline description"];
        }
        return NULL;
    }

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (intern->inFrame) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Cannot create pipeline while renderer is inside a frame"];
        }
        return NULL;
    }

    _ShaderSource vertexShader = _pipeline_get_vertex_shader(desc->shader);
    _ShaderSource pixelShader = _pipeline_get_pixel_shader(desc->shader);

    if (!vertexShader.data || !pixelShader.data) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Shader source is null"];
        }
        return NULL;
    }

    ID3DBlob* vertexBlob = nullptr;
    ID3DBlob* pixelBlob = nullptr;

#if GFX_HLSL_SOURCE_FALLBACK
    vertexBlob = _pipeline_compile_shader(vertexShader);
    pixelBlob = _pipeline_compile_shader(pixelShader);

    if (!vertexBlob || !pixelBlob) {
        if (vertexBlob) vertexBlob->Release();
        if (pixelBlob) pixelBlob->Release();
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Failed to compile HLSL shader"];
        }
        return NULL;
    }
#else
    HRESULT hr = D3DCreateBlob(vertexShader.size, &vertexBlob);
    if (FAILED(hr)) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Failed to create vertex shader blob"];
        }
        return NULL;
    }
    memcpy(vertexBlob->GetBufferPointer(), vertexShader.data, vertexShader.size);

    hr = D3DCreateBlob(pixelShader.size, &pixelBlob);
    if (FAILED(hr)) {
        vertexBlob->Release();
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Failed to create pixel shader blob"];
        }
        return NULL;
    }
    memcpy(pixelBlob->GetBufferPointer(), pixelShader.data, pixelShader.size);
#endif

    RYInternalPipeline* pipeline = new RYInternalPipeline;
    pipeline->vertexLayout = desc->vertexLayout;

    HRESULT hr = intern->device->CreateVertexShader(
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(),
        nullptr,
        &pipeline->vertexShader
    );

    if (FAILED(hr)) {
        vertexBlob->Release();
        if (pixelBlob) pixelBlob->Release();
        delete pipeline;
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Failed to create D3D11 vertex shader"];
        }
        return NULL;
    }

    hr = intern->device->CreatePixelShader(
        pixelBlob->GetBufferPointer(),
        pixelBlob->GetBufferSize(),
        nullptr,
        &pipeline->pixelShader
    );

    if (FAILED(hr)) {
        vertexBlob->Release();
        pixelBlob->Release();
        _pipeline_release(pipeline);
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Failed to create D3D11 pixel shader"];
        }
        return NULL;
    }

    if (!_pipeline_create_input_layout(desc->vertexLayout, *intern, *pipeline, vertexBlob, hr) || FAILED(hr)) {
        vertexBlob->Release();
        pixelBlob->Release();
        _pipeline_release(pipeline);
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Failed to create D3D11 input layout"];
        }
        return NULL;
    }

    vertexBlob->Release();
    pixelBlob->Release();

    if (desc->shader == RYBuiltinShaderStencil) {
        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
        HRESULT blendHr = intern->device->CreateBlendState(&blendDesc, &pipeline->blendState);
        if (FAILED(blendHr)) {
            _pipeline_release(pipeline);
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                               message:"Failed to create stencil blend state"];
            }
            return NULL;
        }
    } else {
        if (!_pipeline_create_blend_state(intern->device, desc->blendMode, &pipeline->blendState)) {
            _pipeline_release(pipeline);
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                               message:"Failed to create blend state"];
            }
            return NULL;
        }
    }

    return pipeline;
}

- (void)destroyPipeline:(void *)pipelineHandle {
    if (!pipelineHandle) return;
    _pipeline_release((RYInternalPipeline *)pipelineHandle);
}

@end
