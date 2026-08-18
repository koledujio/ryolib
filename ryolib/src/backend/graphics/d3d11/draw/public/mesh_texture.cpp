#include "../private.h"

namespace ryo {
    bool draw_mesh_texture(Renderer renderer, Buffer vertexBuffer, Buffer indexBuffer, uint32_t indexCount, Texture texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");
        RYO_ENSURE(buffer_is_valid(vertexBuffer), false, "Invalid vertex buffer handle");
        RYO_ENSURE(buffer_is_valid(indexBuffer), false, "Invalid index buffer handle");
        RYO_ENSURE(indexCount > 0, false, "Index count is 0");
        RYO_ENSURE(texture_is_valid(texture), false, "Invalid texture handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];
        RYO_ENSURE(ir.inPass, false, "Renderer not in a draw pass");

        Pipeline pipeline = _draw_select_pipeline(settings, ir.defaultTexturePipeline);
        RYO_ASSERT(pipeline_is_valid(pipeline), false, "Failed to get a valid pipeline");

        int width = 0;
        int height = 0;
        _draw_get_target_size(ir, width, height);

        Matrix4 mvp = _draw_get_mvp(ir, settings, matrix, width, height);

        if (!_draw_bind_pipeline(ir, pipeline, settings) || !_draw_bind_mvp(ir, mvp) || !_draw_bind_color(ir, color))
            return false;

        _InternalBuffer& vertexIb = _buffer_pool.items[vertexBuffer.index()];
        _InternalBuffer& indexIb = _buffer_pool.items[indexBuffer.index()];
        _InternalTexture& it = _texture_pool.items[texture.index()];

        ID3D11SamplerState* sampler = _draw_create_sampler(ir.device, samplerType);
        if (!sampler)
            return false;

        UINT stride = sizeof(PositionUVVertex);
        UINT offset = 0;

        ir.context->IASetVertexBuffers(0, 1, &vertexIb.buffer, &stride, &offset);
        ir.context->IASetIndexBuffer(indexIb.buffer, DXGI_FORMAT_R32_UINT, 0);
        ir.context->PSSetShaderResources(0, 1, &it.srv);
        ir.context->PSSetSamplers(0, 1, &sampler);
        ir.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ir.context->DrawIndexed(indexCount, 0, 0);

        ID3D11ShaderResourceView* nullSrv = nullptr;
        ir.context->PSSetShaderResources(0, 1, &nullSrv);

        if (sampler) {
            sampler->Release();
        }

        return true;
    }
}