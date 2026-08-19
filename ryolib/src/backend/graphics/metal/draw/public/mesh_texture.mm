#include "../private.h"

namespace ryo {
    bool draw_mesh_texture(Renderer renderer, Buffer vertexBuffer, Buffer indexBuffer, uint32_t indexCount, Texture texture, SamplerType samplerType, const Color& color, const Matrix4 &matrix, const DrawSettings* settings) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");
        RYO_ENSURE(buffer_is_valid(vertexBuffer), false, "Invalid vertex buffer handle");
        RYO_ENSURE(buffer_is_valid(indexBuffer), false, "Invalid index buffer handle");
        RYO_ENSURE(texture_is_valid(texture), false, "Invalid texture handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];
        RYO_ENSURE(ir.inPass && ir.currentEncoder, false, "Renderer not in a draw_pass_begin / draw_pass_end block");
        RYO_ASSERT(ir.currentCommandBuffer && ir.currentDrawable, false, "Renderer has no valid currentCommandBuffer or currentDrawable");

        id<MTLRenderPipelineState> pso = _draw_get_pso(settings, ir.defaultTexturePipeline);
        RYO_ASSERT(pso != nil, false, "Failed to get valid pipeline state");

        id<MTLRenderCommandEncoder> encoder = ir.currentEncoder;
        id<MTLBuffer> vbo = _buffer_pool.items[vertexBuffer.index()];

        [encoder setRenderPipelineState:pso];
        [encoder setDepthStencilState:ir.depthStencil];
        [encoder setVertexBuffer:vbo offset:0 atIndex:0];

        if (settings && settings->camera) {
            int width, height;
            _draw_get_target_size(ir, width, height);
            float fWidth = (float)width;
            float fHeight = (float)height;
            [encoder setVertexBytes:(_compute_camera_vp(*settings->camera, fWidth / fHeight) * matrix).m length:sizeof(Matrix4) atIndex:1];
        } else {
            [encoder setVertexBytes:matrix.m length:sizeof(Matrix4) atIndex:1];
        }

        _draw_apply_settings(encoder, settings);

        simd_float4 col = {color.r, color.g, color.b, color.a};
        [encoder setFragmentBytes:&col length:sizeof(simd_float4) atIndex:0];

        id<MTLTexture> imageToDraw = _texture_pool.items[texture.index()];
        [encoder setFragmentTexture:imageToDraw atIndex:0];
        [encoder setFragmentSamplerState:_sampler_get_from_type(ir, samplerType) atIndex:0];

        id<MTLBuffer> ibo = _buffer_pool.items[indexBuffer.index()];
        [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:indexCount indexType:MTLIndexTypeUInt32 indexBuffer:ibo indexBufferOffset:0];

        return true;
    }
}