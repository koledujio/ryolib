#include "../private.h"

namespace ryo {
    bool Renderer::draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(texture.valid(), false, "Invalid texture");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        const Pipeline* pipeline = _draw_select_pipeline(settings, internal->defaultTexturePipeline);
        RYO_ASSERT(pipeline->valid(), false, "Failed to get a valid pipeline");

        PositionUVVertex vertices[4] = {
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } },
            { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } }
        };

        ID3D11Buffer* vertexBuffer = nullptr;

        if (!_draw_create_dynamic_buffer(internal->device, vertices, sizeof(vertices), D3D11_BIND_VERTEX_BUFFER, &vertexBuffer))
            return false;

        int width = 0;
        int height = 0;
        _draw_get_target_size(*internal, width, height);

        Matrix4 mvp = _draw_get_mvp(*internal, settings, matrix, width, height);

        if (!_draw_bind_pipeline(*internal, pipeline, settings) || !_draw_bind_mvp(*internal, mvp) || !_draw_bind_color(*internal, color)) {
            vertexBuffer->Release();
            return false;
        }

        ID3D11SamplerState* sampler = _draw_create_sampler(internal->device, samplerType);
        if (!sampler)
            return false;

        UINT stride = sizeof(PositionUVVertex);
        UINT offset = 0;

        internal->context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        internal->context->PSSetShaderResources(0, 1, &texture.internal->srv);
        internal->context->PSSetSamplers(0, 1, &sampler);
        internal->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        internal->context->Draw(4, 0);

        ID3D11ShaderResourceView* nullSrv = nullptr;
        internal->context->PSSetShaderResources(0, 1, &nullSrv);

        if (sampler) {
            sampler->Release();
        }

        vertexBuffer->Release();

        return true;
    }
}