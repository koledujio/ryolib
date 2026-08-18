#include "../private.h"

namespace ryo {
    bool draw_rect_color(Renderer renderer, const Color& color, const Matrix4& matrix, const DrawSettings* settings) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");
        
        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];
        RYO_ENSURE(ir.inPass, false, "Renderer not in a draw pass");

        Pipeline pipeline = _draw_select_pipeline(settings, ir.defaultColorPipeline);
        RYO_ASSERT(pipeline_is_valid(pipeline), false, "Failed to get a valid pipeline");

        PositionVertex vertices[4] = {
            { { -0.5f, -0.5f, 0.0f } },
            { {  0.5f, -0.5f, 0.0f } },
            { { -0.5f,  0.5f, 0.0f } },
            { {  0.5f,  0.5f, 0.0f } }
        };

        ID3D11Buffer* vertexBuffer = nullptr;
        if (!_draw_create_dynamic_buffer(ir.device, vertices, sizeof(vertices), D3D11_BIND_VERTEX_BUFFER, &vertexBuffer))
            return false;

        int width = 0;
        int height = 0;
        _draw_get_target_size(ir, width, height);

        Matrix4 mvp = _draw_get_mvp(ir, settings, matrix, width, height);

        if (!_draw_bind_pipeline(ir, pipeline, settings) || !_draw_bind_mvp(ir, mvp) || !_draw_bind_color(ir, color)) {
            vertexBuffer->Release();
            return false;
        }

        UINT stride = sizeof(PositionVertex);
        UINT offset = 0;

        ir.context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        ir.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        ir.context->Draw(4, 0);
        
        vertexBuffer->Release();

        return true;
    }
}