#include "../private.h"

namespace ryo {
    bool Renderer::draw_rect_color(const Color &color, const Matrix4 &matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        id<MTLRenderPipelineState> pso = _draw_get_pso(settings, internal->defaultColorPipeline);
        RYO_ASSERT(pso != nil, false, "Failed to get valid pipeline state");

        id<MTLRenderCommandEncoder> encoder = internal->currentEncoder;

        PositionVertex vertices[4] = {
            { {-0.5f, -0.5f, 0.0f } },
            { { 0.5f, -0.5f, 0.0f } },
            { {-0.5f,  0.5f, 0.0f } },
            { { 0.5f,  0.5f, 0.0f } },
        };

        [encoder setRenderPipelineState:pso];
        [encoder setDepthStencilState:internal->depthStencilState];
        [encoder setVertexBytes:vertices length:sizeof(vertices) atIndex:0];

        if (settings && settings->camera) {
            int width, height;
            _draw_get_target_size(*internal, width, height);
            float fWidth = (float)width;
            float fHeight = (float)height;
            [encoder setVertexBytes:(_compute_camera_vp(*settings->camera, fWidth / fHeight) * matrix).m length:sizeof(Matrix4) atIndex:1];
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