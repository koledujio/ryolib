#include "../private.h"

@implementation RYMetalGraphicsBackend (DrawCall)

- (BOOL)drawEx:(const RYDrawCall *)draw {
    if (!_internal || !draw) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (!intern->inFrame || !intern->inPass) return NO;
    if (!draw->pipeline) return NO;
    if (!draw->vertexBuffer) return NO;
    if (!draw->indexBuffer) return NO;
    if (draw->indexCount == 0) return NO;

    bool useTextures = false;
    bool useVertexUniform = false;
    bool useFragmentUniform = false;
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
        useVertexUniform = true;
    }

    if (draw->fragmentUniformDataSize > 0) {
        if (!draw->fragmentUniformData) return NO;
        useFragmentUniform = true;
    }

    if (draw->stencil != nil && !draw->stencil.isEmpty) {
        useStencil = true;
    }

    id<MTLRenderCommandEncoder> encoder = intern->currentEncoder;

    int depthIndex = static_cast<int>(draw->depthMode);
    if (depthIndex < 0 || depthIndex > 2)
        depthIndex = static_cast<int>(RYDepthModeReadWrite);

    if (useStencil) {
        [encoder setDepthStencilState:intern->stencilWriteState];
        [encoder setStencilReferenceValue:1];
        RYInternalPipeline *stencilPipe = (RYInternalPipeline *)intern->stencilPipeline.internalPipeline;
        [encoder setRenderPipelineState:stencilPipe->pso];
        RYInternalBuffer *quadVb = (RYInternalBuffer *)intern->quadVertexBuffer.internalBuffer;
        [encoder setVertexBuffer:quadVb->buffer offset:0 atIndex:0];

        int width = 0, height = 0;
        _draw_get_target_size(*intern, width, height);
        float aspect = (height > 0) ? ((float)width / (float)height) : 1.0f;

        RYInternalStencil *stencilIntern = (RYInternalStencil *)draw->stencil.internalStencil;
        for (size_t i = 0; i < stencilIntern->commandCount; i++) {
            const RYStencilCommand &command = stencilIntern->commands[i];

            ryo::Matrix4 mvp = command.matrix;
            if (command.hasCamera) {
                mvp = _compute_camera_vp(command.camera, aspect) * command.matrix;
            }

            RYStencilVertexUniforms vertexUniforms;
            vertexUniforms.matrix = mvp;

            float sx = sqrtf(command.matrix.m[0] * command.matrix.m[0] + command.matrix.m[1] * command.matrix.m[1] + command.matrix.m[2] * command.matrix.m[2]);
            float sy = sqrtf(command.matrix.m[4] * command.matrix.m[4] + command.matrix.m[5] * command.matrix.m[5] + command.matrix.m[6] * command.matrix.m[6]);
            if (sx <= 0.0f) sx = 1.0f;
            if (sy <= 0.0f) sy = 1.0f;

            RYStencilFragmentUniforms fragmentUniforms;
            fragmentUniforms.scale = {sx, sy};
            fragmentUniforms.radius = command.radius;
            fragmentUniforms.shape = command.shape;

            [encoder setVertexBytes:&vertexUniforms length:sizeof(vertexUniforms) atIndex:1];
            [encoder setFragmentBytes:&fragmentUniforms length:sizeof(fragmentUniforms) atIndex:0];

            RYInternalBuffer *quadIb = (RYInternalBuffer *)intern->quadIndexBuffer.internalBuffer;
            [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                     indexCount:6
                     indexType:MTLIndexTypeUInt32
                     indexBuffer:quadIb->buffer
                     indexBufferOffset:0];
        }

        [encoder setDepthStencilState:intern->stencilTestStates[depthIndex]];
        [encoder setStencilReferenceValue:1];
    } else {
        [encoder setDepthStencilState:intern->depthStates[depthIndex]];
    }

    MTLCullMode mtlCull = (draw->cullMode == RYCullModeNone) ? MTLCullModeNone :
                          (draw->cullMode == RYCullModeFront) ? MTLCullModeFront : MTLCullModeBack;
    [encoder setCullMode:mtlCull];

    MTLWinding mtlWinding = (draw->windingOrder == RYWindingOrderClockwise) ? MTLWindingClockwise : MTLWindingCounterClockwise;
    [encoder setFrontFacingWinding:mtlWinding];

    MTLTriangleFillMode mtlFill = (draw->fillMode == RYFillModeWireframe) ? MTLTriangleFillModeLines : MTLTriangleFillModeFill;
    [encoder setTriangleFillMode:mtlFill];

    RYInternalPipeline *drawPipe = (RYInternalPipeline *)draw->pipeline.internalPipeline;
    id<MTLRenderPipelineState> pipelineState = drawPipe->pso;
    [encoder setRenderPipelineState:pipelineState];

    RYInternalBuffer *drawVb = (RYInternalBuffer *)draw->vertexBuffer.internalBuffer;
    id<MTLBuffer> vertexBuffer = drawVb->buffer;
    [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

    if (useVertexUniform)
        [encoder setVertexBytes:draw->vertexUniformData length:draw->vertexUniformDataSize atIndex:1];

    if (useFragmentUniform)
        [encoder setFragmentBytes:draw->fragmentUniformData length:draw->fragmentUniformDataSize atIndex:0];

    if (useTextures) {
        for (uint32_t i = 0; i < draw->textureCount; i++) {
            RYInternalTexture *texIntern = (RYInternalTexture *)draw->textures[i].internalTexture;
            [encoder setFragmentTexture:texIntern->texture atIndex:i];
            [encoder setFragmentSamplerState:_sampler_get_from_type(*intern, draw->samplerTypes[i]) atIndex:i];
        }
    }

    RYInternalBuffer *drawIb = (RYInternalBuffer *)draw->indexBuffer.internalBuffer;
    id<MTLBuffer> indexBuffer = drawIb->buffer;
    [encoder
        drawIndexedPrimitives:MTLPrimitiveTypeTriangle
        indexCount:draw->indexCount
        indexType:MTLIndexTypeUInt32
        indexBuffer:indexBuffer
        indexBufferOffset:0
    ];

    return YES;
}

@end
