#include "../private.h"

namespace ryo {
    bool Renderer::draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4 &matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(vertexBuffer.valid(), false, "Invalid vertex buffer");
        RYO_ENSURE(indexBuffer.valid(), false, "Invalid index buffer");
        RYO_ENSURE(texture.valid(), false, "Invalid texture");

        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        id<MTLRenderPipelineState> pso = _draw_get_pso(settings, internal->defaultTexturePipeline);
        RYO_ASSERT(pso != nil, false, "Failed to get valid pipeline state");

        id<MTLRenderCommandEncoder> encoder = internal->currentEncoder;
        id<MTLBuffer> vbo = vertexBuffer.internal->buffer;

        [encoder setRenderPipelineState:pso];
        [encoder setDepthStencilState:internal->depthStencilState];
        [encoder setVertexBuffer:vbo offset:0 atIndex:0];

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

        id<MTLTexture> imageToDraw = texture.internal->texture;
        [encoder setFragmentTexture:imageToDraw atIndex:0];
        [encoder setFragmentSamplerState:_sampler_get_from_type(*internal, samplerType) atIndex:0];

        id<MTLBuffer> ibo = indexBuffer.internal->buffer;
        [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:indexCount indexType:MTLIndexTypeUInt32 indexBuffer:ibo indexBufferOffset:0];

        return true;
    }
}