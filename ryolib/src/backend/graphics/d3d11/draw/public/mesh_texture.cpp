#include "../private.h"

namespace ryo {
    bool Renderer::draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(vertexBuffer.valid(), false, "Invalid vertex buffer");
        RYO_ENSURE(indexBuffer.valid(), false, "Invalid index buffer");
        RYO_ENSURE(indexCount > 0, false, "Index count is 0");
        RYO_ENSURE(texture.valid(), false, "Invalid texture");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        const Pipeline* pipeline = _draw_select_pipeline(settings, internal->defaultTexturePipeline);
        RYO_ASSERT(pipeline->valid(), false, "Failed to get a valid pipeline");

        int width = 0;
        int height = 0;
        _draw_get_target_size(*internal, width, height);

        Matrix4 mvp = _draw_get_mvp(*internal, settings, matrix, width, height);

        if (!_draw_bind_pipeline(*internal, pipeline, settings) || !_draw_bind_mvp(*internal, mvp) || !_draw_bind_color(*internal, color))
            return false;

        ID3D11SamplerState* sampler = _draw_create_sampler(internal->device, samplerType);
        if (!sampler)
            return false;

        UINT stride = sizeof(PositionUVVertex);
        UINT offset = 0;

        internal->context->IASetVertexBuffers(0, 1, &vertexBuffer.internal->buffer, &stride, &offset);
        internal->context->IASetIndexBuffer(indexBuffer.internal->buffer, DXGI_FORMAT_R32_UINT, 0);
        internal->context->PSSetShaderResources(0, 1, &texture.internal->srv);
        internal->context->PSSetSamplers(0, 1, &sampler);
        internal->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        internal->context->DrawIndexed(indexCount, 0, 0);

        ID3D11ShaderResourceView* nullSrv = nullptr;
        internal->context->PSSetShaderResources(0, 1, &nullSrv);

        if (sampler)
            sampler->Release();

        return true;
    }
}