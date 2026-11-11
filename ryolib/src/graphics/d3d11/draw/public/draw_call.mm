#include "../private.h"
#include <cmath>

void _update_constant_buffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const void* data, size_t size) {
    if (!data || size == 0) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        memcpy(mappedResource.pData, data, size);
        context->Unmap(buffer, 0);
    }
}

@implementation RYD3D11GraphicsBackend (DrawCall)

- (BOOL)drawEx:(const RYDrawCall *)draw {
    if (!_internal || !draw) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (!intern->inFrame || !intern->inPass) return NO;
    if (!draw->pipeline) return NO;
    if (!draw->vertexBuffer) return NO;
    if (!draw->indexBuffer) return NO;
    if (draw->indexCount == 0) return NO;

    bool useTextures = false;
    bool useVertexUniforms = false;
    bool usePixelUniforms = false;
    bool useStencil = false;

    if (draw->textureCount > 0) {
        useTextures = true;
        if (!draw->textures || !draw->samplerTypes) return NO;
        for (uint32_t i = 0; i < draw->textureCount; i++) {
            if (!draw->textures[i]) return NO;
        }
    }

    if (draw->vertexUniformDataSize > 0) {
        if (!draw->vertexUniformData) return NO;
        useVertexUniforms = true;
    }

    if (draw->fragmentUniformDataSize > 0) {
        if (!draw->fragmentUniformData) return NO;
        usePixelUniforms = true;
    }

    if (draw->stencil != nil && !draw->stencil.isEmpty) {
        useStencil = true;
    }

    ID3D11DeviceContext* context = intern->context;

    int depthIndex = static_cast<int>(draw->depthMode);
    if (depthIndex < 0 || depthIndex > 2)
        depthIndex = static_cast<int>(RYDepthModeReadWrite);

    if (useStencil) {
        context->OMSetDepthStencilState(intern->stencilWriteState, 1);
        RYInternalPipeline* stencilPipe = (RYInternalPipeline*)intern->stencilPipeline.internalPipeline;
        context->OMSetBlendState(stencilPipe->blendState, nullptr, 0xFFFFFFFF);

        context->VSSetShader(stencilPipe->vertexShader, nullptr, 0);
        context->PSSetShader(stencilPipe->pixelShader, nullptr, 0);
        context->IASetInputLayout(stencilPipe->inputLayout);

        UINT stride = sizeof(RYPositionVertex);
        UINT offset = 0;
        RYInternalBuffer* quadVb = (RYInternalBuffer*)intern->quadVertexBuffer.internalBuffer;
        ID3D11Buffer* d3dVb = quadVb->buffer;
        context->IASetVertexBuffers(0, 1, &d3dVb, &stride, &offset);

        RYInternalBuffer* quadIb = (RYInternalBuffer*)intern->quadIndexBuffer.internalBuffer;
        context->IASetIndexBuffer(quadIb->buffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        int width = 0, height = 0;
        _draw_get_target_size(*intern, width, height);
        float aspect = (height > 0) ? ((float)width / (float)height) : 1.0f;

        RYInternalStencil* stencilIntern = (RYInternalStencil*)draw->stencil.internalStencil;
        for (size_t i = 0; i < stencilIntern->commandCount; i++) {
            const RYStencilCommand& command = stencilIntern->commands[i];

            ryo::Matrix4 mvp = command.matrix;
            if (command.hasCamera) {
                mvp = _draw_compute_camera_vp(command.camera, aspect) * command.matrix;
            }

            RYStencilVertexUniforms vertexUniforms;
            vertexUniforms.matrix = mvp;

            float sx = sqrtf(command.matrix.m[0] * command.matrix.m[0] + command.matrix.m[1] * command.matrix.m[1] + command.matrix.m[2] * command.matrix.m[2]);
            float sy = sqrtf(command.matrix.m[4] * command.matrix.m[4] + command.matrix.m[5] * command.matrix.m[5] + command.matrix.m[6] * command.matrix.m[6]);
            if (sx <= 0.0f) sx = 1.0f;
            if (sy <= 0.0f) sy = 1.0f;

            RYStencilPixelUniforms pixelUniforms;
            pixelUniforms.scale = {sx, sy};
            pixelUniforms.radius = command.radius;
            pixelUniforms.shape = command.shape;

            _update_constant_buffer(context, intern->vertexCB, &vertexUniforms, sizeof(RYStencilVertexUniforms));
            ID3D11Buffer* vCbs[] = { intern->vertexCB };
            context->VSSetConstantBuffers(0, 1, vCbs);

            _update_constant_buffer(context, intern->pixelCB, &pixelUniforms, sizeof(RYStencilPixelUniforms));
            ID3D11Buffer* pCbs[] = { intern->pixelCB };
            context->PSSetConstantBuffers(0, 1, pCbs);

            context->DrawIndexed(6, 0, 0);
        }

        context->OMSetDepthStencilState(intern->stencilTestStates[depthIndex], 1);
    } else {
        context->OMSetDepthStencilState(intern->depthStates[depthIndex], 0);
    }

    ID3D11RasterizerState* rsState = _get_rasterizer_state(*intern, draw->cullMode, draw->fillMode, draw->windingOrder);
    context->RSSetState(rsState);

    RYInternalPipeline* drawPipe = (RYInternalPipeline*)draw->pipeline.internalPipeline;
    context->OMSetBlendState(drawPipe->blendState, nullptr, 0xFFFFFFFF);

    context->VSSetShader(drawPipe->vertexShader, nullptr, 0);
    context->PSSetShader(drawPipe->pixelShader, nullptr, 0);
    context->IASetInputLayout(drawPipe->inputLayout);

    if (useVertexUniforms) {
        _update_constant_buffer(context, intern->vertexCB, draw->vertexUniformData, draw->vertexUniformDataSize);
        ID3D11Buffer* vCbs[] = { intern->vertexCB };
        context->VSSetConstantBuffers(0, 1, vCbs);
    }

    if (usePixelUniforms) {
        _update_constant_buffer(context, intern->pixelCB, draw->fragmentUniformData, draw->fragmentUniformDataSize);
        ID3D11Buffer* pCbs[] = { intern->pixelCB };
        context->PSSetConstantBuffers(0, 1, pCbs);
    }

    if (useTextures) {
        for (uint32_t i = 0; i < draw->textureCount; i++) {
            RYInternalTexture* texIntern = (RYInternalTexture*)draw->textures[i].internalTexture;
            ID3D11ShaderResourceView* srv = texIntern->srv;
            context->PSSetShaderResources(i, 1, &srv);

            ID3D11SamplerState* sampler = _sampler_get_from_type(*intern, draw->samplerTypes[i]);
            context->PSSetSamplers(i, 1, &sampler);
        }
    }

    UINT stride = draw->vertexStride;
    UINT offset = 0;
    RYInternalBuffer* vbIntern = (RYInternalBuffer*)draw->vertexBuffer.internalBuffer;
    ID3D11Buffer* vBuffer = vbIntern->buffer;
    context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

    RYInternalBuffer* ibIntern = (RYInternalBuffer*)draw->indexBuffer.internalBuffer;
    context->IASetIndexBuffer(ibIntern->buffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->DrawIndexed(draw->indexCount, 0, 0);

    return YES;
}

@end
