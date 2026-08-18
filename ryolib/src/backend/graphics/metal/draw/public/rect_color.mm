#include "../private.h"

namespace ryo {
    bool draw_rect_color(Renderer renderer, const Color &color, const Matrix4 &matrix, const DrawSettings* settings) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];
        RYO_ENSURE(ir.inPass && ir.currentEncoder, false, "Renderer not in a draw_pass_begin / draw_pass_end block");
        RYO_ASSERT(ir.currentCommandBuffer && ir.currentDrawable, false, "Renderer has no valid currentCommandBuffer or currentDrawable");

        id<MTLRenderPipelineState> pso = _draw_get_pso(settings, ir.defaultColorPipeline);
        RYO_ASSERT(pso != nil, false, "Failed to get valid pipeline state");

        id<MTLRenderCommandEncoder> encoder = ir.currentEncoder;

        PositionVertex vertices[4] = {
            { {-0.5f, -0.5f, 0.0f } },
            { { 0.5f, -0.5f, 0.0f } },
            { {-0.5f,  0.5f, 0.0f } },
            { { 0.5f,  0.5f, 0.0f } },
        };

        [encoder setRenderPipelineState:pso];
        [encoder setDepthStencilState:ir.depthStencil];
        [encoder setVertexBytes:vertices length:sizeof(vertices) atIndex:0];

        if (settings && settings->camera) {
            int width, height;
            _draw_get_target_size(ir, width, height);
            float fWidth = (float)width;
            float fHeight = (float)height;
            [encoder setVertexBytes:matrix4_multiply(_compute_camera_vp(*settings->camera, fWidth / fHeight), matrix).m length:sizeof(Matrix4) atIndex:1];
        } else {
            [encoder setVertexBytes:matrix.m length:sizeof(Matrix4) atIndex:1];
        }

        _draw_apply_settings(encoder, settings);

        simd_float4 col = {color.r, color.g, color.b, color.a};
        [encoder setFragmentBytes:&col length:sizeof(simd_float4) atIndex:0];

        [encoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

        return true;
    }
}