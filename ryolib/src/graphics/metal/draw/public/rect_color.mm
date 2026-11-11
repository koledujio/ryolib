#include "../private.h"

@implementation RYMetalGraphicsBackend (RectColor)

- (BOOL)drawRectColor:(RYColor)color matrix:(ryo::Matrix4)matrix settings:(const RYDrawSettings *)settings {
    if (!_internal) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;

    ryo::Matrix4 mvp = matrix;
    if (settings && settings->camera) {
        int width, height;
        _draw_get_target_size(*intern, width, height);
        float aspect = (height > 0) ? ((float)width / (float)height) : 1.0f;
        mvp = _compute_camera_vp(*settings->camera, aspect) * matrix;
    }

    simd_float4 col = {color.r, color.g, color.b, color.a};

    RYPipeline *pipeline = (settings && settings->pipelineOverride)
        ? settings->pipelineOverride
        : intern->defaultColorPipeline;

    RYDrawCall call;
    call.pipeline = pipeline;
    call.vertexBuffer = intern->quadVertexBuffer;
    call.vertexStride = sizeof(RYPositionVertex);
    call.indexBuffer = intern->quadIndexBuffer;
    call.indexCount = 6;
    call.textures = nullptr;
    call.samplerTypes = nullptr;
    call.textureCount = 0;
    call.vertexUniformData = mvp.m;
    call.vertexUniformDataSize = sizeof(ryo::Matrix4);
    call.fragmentUniformData = &col;
    call.fragmentUniformDataSize = sizeof(simd_float4);
    call.cullMode = settings ? settings->cullMode : RYCullModeNone;
    call.windingOrder = settings ? settings->windingOrder : RYWindingOrderCounterClockwise;
    call.fillMode = settings ? settings->fillMode : RYFillModeSolid;
    call.depthMode = settings ? settings->depthMode : RYDepthModeReadWrite;
    call.stencil = settings ? settings->stencil : nil;

    return [self drawEx:&call];
}

@end
