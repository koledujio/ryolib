#include "../private.h"

namespace ryo {
    bool Renderer::draw_ex(const DrawCall& draw) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");
        RYO_ENSURE(draw.pipeline.valid(), false, "Invalid pipeline");
        RYO_ENSURE(draw.vertexBuffer.valid(), false, "Invalid vertex buffer");
        RYO_ENSURE(draw.indexBuffer.valid(), false, "Invalid index buffer");
        RYO_ENSURE(draw.indexCount > 0, false, "Invalid number of indexes");

        bool useTextures = false;
        bool useVertexUniform = false;
        bool useFragmentUniform = false;
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
            useVertexUniform = true;
        }

        if (draw.fragmentUniformDataSize > 0) {
            RYO_ENSURE(draw.fragmentUniformData != nullptr, false, "Invalid fragment uniform data");
            useFragmentUniform = true;
        }

        if (draw.stencil != nullptr && !draw.stencil->empty()) {
            RYO_ENSURE(draw.stencil->valid(), false, "Invalid stencil");
            useStencil = !draw.stencil->empty();
        }

        id<MTLRenderCommandEncoder> encoder = internal->currentEncoder;

        int depthIndex = static_cast<int>(draw.depthMode);
        if (depthIndex < 0 || depthIndex > 2)
            depthIndex = static_cast<int>(DEPTH_MODE_READ_WRITE);

        if (useStencil) {
            [encoder setDepthStencilState:internal->stencilWriteState];
            [encoder setStencilReferenceValue:1];
            [encoder setRenderPipelineState:internal->stencilPipeline->internal->pso];
            [encoder setVertexBuffer:internal->quadVertexBuffer->internal->buffer offset:0 atIndex:0];

            int width = 0, height = 0;
            _draw_get_target_size(*internal, width, height);
            float aspect = (height > 0) ? ((float)width / (float)height) : 1.0f;

            for (size_t i = 0; i < draw.stencil->internal->commandCount; i++) {
                const StencilCommand& command = draw.stencil->internal->commands[i];

                Matrix4 mvp = command.matrix;
                if (command.hasCamera) {
                    mvp = _compute_camera_vp(command.camera, aspect) * command.matrix;
                }

                StencilVertexUniforms vertexUniforms;
                vertexUniforms.matrix = mvp;

                float sx = sqrtf(command.matrix.m[0] * command.matrix.m[0] + command.matrix.m[1] * command.matrix.m[1] + command.matrix.m[2] * command.matrix.m[2]);
                float sy = sqrtf(command.matrix.m[4] * command.matrix.m[4] + command.matrix.m[5] * command.matrix.m[5] + command.matrix.m[6] * command.matrix.m[6]);
                if (sx <= 0.0f) sx = 1.0f;
                if (sy <= 0.0f) sy = 1.0f;

                StencilFragmentUniforms fragmentUniforms;
                fragmentUniforms.scale = {sx, sy};
                fragmentUniforms.radius = command.radius;
                fragmentUniforms.shape = command.shape;

                [encoder setVertexBytes:&vertexUniforms length:sizeof(vertexUniforms) atIndex:1];
                [encoder setFragmentBytes:&fragmentUniforms length:sizeof(fragmentUniforms) atIndex:0];

                [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                         indexCount:6
                         indexType:MTLIndexTypeUInt32
                         indexBuffer:internal->quadIndexBuffer->internal->buffer
                         indexBufferOffset:0];
            }

            [encoder setDepthStencilState:internal->stencilTestStates[depthIndex]];
            [encoder setStencilReferenceValue:1];
        } else {
            [encoder setDepthStencilState:internal->depthStates[depthIndex]];
        }

        MTLCullMode mtlCull = (draw.cullMode == CULL_MODE_NONE) ? MTLCullModeNone :
                              (draw.cullMode == CULL_MODE_FRONT) ? MTLCullModeFront : MTLCullModeBack;
        [encoder setCullMode:mtlCull];

        MTLWinding mtlWinding = (draw.windingOrder == WINDING_ORDER_CLOCKWISE) ? MTLWindingClockwise : MTLWindingCounterClockwise;
        [encoder setFrontFacingWinding:mtlWinding];

        MTLTriangleFillMode mtlFill = (draw.fillMode == FILL_MODE_WIREFRAME) ? MTLTriangleFillModeLines : MTLTriangleFillModeFill;
        [encoder setTriangleFillMode:mtlFill];

        id<MTLRenderPipelineState> pipelineState = draw.pipeline.internal->pso;
        [encoder setRenderPipelineState:pipelineState];

        id<MTLBuffer> vertexBuffer = draw.vertexBuffer.internal->buffer;
        [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

        if (useVertexUniform)
            [encoder setVertexBytes:draw.vertexUniformData length:draw.vertexUniformDataSize atIndex:1];

        if (useFragmentUniform)
            [encoder setFragmentBytes:draw.fragmentUniformData length:draw.fragmentUniformDataSize atIndex:0];

        if (useTextures) {
            for (int i = 0; i < draw.textureCount; i++) {
                [encoder setFragmentTexture:draw.textures[i]->internal->texture atIndex:i];
                [encoder setFragmentSamplerState:_sampler_get_from_type(*internal, draw.samplerTypes[i]) atIndex:i];
            }
        }

        id<MTLBuffer> indexBuffer = draw.indexBuffer.internal->buffer;
        [encoder
            drawIndexedPrimitives:MTLPrimitiveTypeTriangle
            indexCount:draw.indexCount
            indexType:MTLIndexTypeUInt32
            indexBuffer:indexBuffer
            indexBufferOffset:0
        ];

        return true;
    }
}