#include "../private.h"

#include <cmath>

namespace ryo {
    static void _update_constant_buffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const void* data, size_t size) {
        if (!data || size == 0) return;
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (SUCCEEDED(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
            memcpy(mappedResource.pData, data, size);
            context->Unmap(buffer, 0);
        }
    }

    static ID3D11SamplerState* _sampler_get_from_type(InternalRenderer& intern, SamplerType type) {
        int index = static_cast<int>(type);
        if (intern.samplers[index] != nullptr) {
            return intern.samplers[index];
        }

        D3D11_SAMPLER_DESC desc = {};
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        switch (type) {
            case SAMPLER_TYPE_NEAREST:
                desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                break;
            case SAMPLER_TYPE_LINEAR:
                desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                break;
            case SAMPLER_TYPE_MIN_NEAREST_MAG_LINEAR:
                desc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
                break;
            case SAMPLER_TYPE_MIN_LINEAR_MAG_NEAREST:
                desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
                break;
        }

        intern.device->CreateSamplerState(&desc, &intern.samplers[index]);
        return intern.samplers[index];
    }

    static ID3D11RasterizerState* _get_rasterizer_state(InternalRenderer& intern, CullMode cull, FillMode fill, WindingOrder winding) {
        int c = static_cast<int>(cull);
        int f = static_cast<int>(fill);
        int w = static_cast<int>(winding);

        if (intern.rasterizers[c][f][w] != nullptr) {
            return intern.rasterizers[c][f][w];
        }

        D3D11_RASTERIZER_DESC desc = {};
        desc.CullMode = (cull == CULL_MODE_NONE) ? D3D11_CULL_NONE :
                        (cull == CULL_MODE_FRONT) ? D3D11_CULL_FRONT : D3D11_CULL_BACK;
        desc.FillMode = (fill == FILL_MODE_WIREFRAME) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
        desc.FrontCounterClockwise = (winding == WINDING_ORDER_COUNTER_CLOCKWISE) ? TRUE : FALSE;
        desc.DepthClipEnable = TRUE;

        intern.device->CreateRasterizerState(&desc, &intern.rasterizers[c][f][w]);
        return intern.rasterizers[c][f][w];
    }

    bool Renderer::draw_ex(const DrawCall& draw) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");
        RYO_ENSURE(draw.pipeline.valid(), false, "Invalid pipeline");
        RYO_ENSURE(draw.vertexBuffer.valid(), false, "Invalid vertex buffer");
        RYO_ENSURE(draw.indexBuffer.valid(), false, "Invalid index buffer");
        RYO_ENSURE(draw.indexCount > 0, false, "Invalid number of indexes");

        bool useTextures = false;
        bool useVertexUniforms = false;
        bool usePixelUniforms = false;
        bool useStencil = false;

        if (draw.textureCount > 0) {
            useTextures = true;

#if defined(RYO_DEBUG) || defined(RYO_SAFETY)
            RYO_ENSURE(draw.textures != nullptr, false, "Invalid texture array");
            RYO_ENSURE(draw.samplerTypes != nullptr, false, "Invalid sampler type array");

            for (int i = 0; i < draw.textureCount; i++) {
                RYO_ENSURE(draw.textures[i]->valid(), false, "Invalid texture");
            }
#endif
        }

        if (draw.vertexUniformDataSize > 0) {
            RYO_ENSURE(draw.vertexUniformData != nullptr, false, "Invalid vertex uniform data");
            useVertexUniforms = true;
        }

        if (draw.fragmentUniformDataSize > 0) {
            RYO_ENSURE(draw.fragmentUniformData != nullptr, false, "Invalid fragment uniform data");
            usePixelUniforms = true;
        }

        if (draw.stencil != nullptr && !draw.stencil->empty()) {
            RYO_ENSURE(draw.stencil->valid(), false, "Invalid stencil");
            useStencil = !draw.stencil->empty();
        }

        ID3D11DeviceContext* context = internal->context;

        int depthIndex = static_cast<int>(draw.depthMode);
        if (depthIndex < 0 || depthIndex > 2)
            depthIndex = static_cast<int>(DEPTH_MODE_READ_WRITE);

        if (useStencil) {
            context->OMSetDepthStencilState(internal->stencilWriteState, 1);
            context->OMSetBlendState(internal->stencilPipeline->internal->blendState, nullptr, 0xFFFFFFFF);

            context->VSSetShader(internal->stencilPipeline->internal->vertexShader, nullptr, 0);
            context->PSSetShader(internal->stencilPipeline->internal->pixelShader, nullptr, 0);
            context->IASetInputLayout(internal->stencilPipeline->internal->inputLayout);

            UINT stride = sizeof(PositionVertex);
            UINT offset = 0;
            ID3D11Buffer* quadVb = internal->quadVertexBuffer->internal->buffer;
            context->IASetVertexBuffers(0, 1, &quadVb, &stride, &offset);
            context->IASetIndexBuffer(internal->quadIndexBuffer->internal->buffer, DXGI_FORMAT_R32_UINT, 0);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            int width = 0, height = 0;
            _draw_get_target_size(*internal, width, height);
            float aspect = (height > 0) ? ((float)width / (float)height) : 1.0f;

            for (size_t i = 0; i < draw.stencil->internal->commandCount; i++) {
                const StencilCommand& command = draw.stencil->internal->commands[i];

                Matrix4 mvp = command.matrix;
                if (command.hasCamera) {
                    mvp = _draw_compute_camera_vp(command.camera, aspect) * command.matrix;
                }

                StencilVertexUniforms vertexUniforms;
                vertexUniforms.matrix = mvp;

                float sx = sqrtf(command.matrix.m[0] * command.matrix.m[0] + command.matrix.m[1] * command.matrix.m[1] + command.matrix.m[2] * command.matrix.m[2]);
                float sy = sqrtf(command.matrix.m[4] * command.matrix.m[4] + command.matrix.m[5] * command.matrix.m[5] + command.matrix.m[6] * command.matrix.m[6]);
                if (sx <= 0.0f) sx = 1.0f;
                if (sy <= 0.0f) sy = 1.0f;

                StencilPixelUniforms pixelUniforms;
                pixelUniforms.scale = {sx, sy};
                pixelUniforms.radius = command.radius;
                pixelUniforms.shape = command.shape;

                _update_constant_buffer(context, internal->vertexCB, &vertexUniforms, sizeof(StencilVertexUniforms));
                ID3D11Buffer* vCbs[] = { internal->vertexCB };
                context->VSSetConstantBuffers(0, 1, vCbs);

                _update_constant_buffer(context, internal->pixelCB, &pixelUniforms, sizeof(StencilPixelUniforms));
                ID3D11Buffer* pCbs[] = { internal->pixelCB };
                context->PSSetConstantBuffers(0, 1, pCbs);

                context->DrawIndexed(6, 0, 0);
            }

            context->OMSetDepthStencilState(internal->stencilTestStates[depthIndex], 1);
        } else {
            context->OMSetDepthStencilState(internal->depthStates[depthIndex], 0);
        }

        ID3D11RasterizerState* rsState = _get_rasterizer_state(*internal, draw.cullMode, draw.fillMode, draw.windingOrder);
        context->RSSetState(rsState);

        context->OMSetBlendState(draw.pipeline.internal->blendState, nullptr, 0xFFFFFFFF);

        context->VSSetShader(draw.pipeline.internal->vertexShader, nullptr, 0);
        context->PSSetShader(draw.pipeline.internal->pixelShader, nullptr, 0);
        context->IASetInputLayout(draw.pipeline.internal->inputLayout);

        if (useVertexUniforms) {
            _update_constant_buffer(context, internal->vertexCB, draw.vertexUniformData, draw.vertexUniformDataSize);
            ID3D11Buffer* vCbs[] = { internal->vertexCB };
            context->VSSetConstantBuffers(0, 1, vCbs);
        }

        if (usePixelUniforms) {
            _update_constant_buffer(context, internal->pixelCB, draw.fragmentUniformData, draw.fragmentUniformDataSize);
            ID3D11Buffer* pCbs[] = { internal->pixelCB };
            context->PSSetConstantBuffers(0, 1, pCbs);
        }

        if (useTextures) {
            for (uint32_t i = 0; i < draw.textureCount; i++) {
                ID3D11ShaderResourceView* srv = draw.textures[i]->internal->srv;
                context->PSSetShaderResources(i, 1, &srv);

                ID3D11SamplerState* sampler = _sampler_get_from_type(*internal, draw.samplerTypes[i]);
                context->PSSetSamplers(i, 1, &sampler);
            }
        }

        UINT stride = draw.vertexStride;
        UINT offset = 0;
        ID3D11Buffer* vBuffer = draw.vertexBuffer.internal->buffer;
        context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

        context->IASetIndexBuffer(draw.indexBuffer.internal->buffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->DrawIndexed(draw.indexCount, 0, 0);

        return true;
    }
}