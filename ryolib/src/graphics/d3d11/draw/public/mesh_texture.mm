#include "../private.h"

@implementation RYD3D11GraphicsBackend (MeshTexture)

- (BOOL)drawMeshTextureWithVertexBuffer:(RYBuffer *)vertexBuffer indexBuffer:(RYBuffer *)indexBuffer indexCount:(uint32_t)indexCount texture:(RYTexture *)texture samplerType:(RYSamplerType)samplerType color:(RYColor)color matrix:(ryo::Matrix4)matrix settings:(const RYDrawSettings *)settings {
    if (!_internal || !vertexBuffer || !indexBuffer || !texture) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;

    ryo::Matrix4 mvp = matrix;
    if (settings && settings->camera) {
        int width = 0, height = 0;
        _draw_get_target_size(*intern, width, height);
        float aspect = (height > 0) ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
        mvp = _draw_compute_camera_vp(*settings->camera, aspect) * matrix;
    }

    float col[4] = {color.r, color.g, color.b, color.a};

    RYPipeline *pipeline = (settings && settings->pipelineOverride)
        ? (RYPipeline *)settings->pipelineOverride
        : intern->defaultTexturePipeline;

    RYTexture *textures[] = { texture };
    RYSamplerType samplers[] = { samplerType };

    RYDrawCall call;
    call.pipeline = pipeline;
    call.vertexBuffer = vertexBuffer;
    call.vertexStride = sizeof(RYPositionUVVertex);
    call.indexBuffer = indexBuffer;
    call.indexCount = indexCount;
    call.textures = textures;
    call.samplerTypes = samplers;
    call.textureCount = 1;
    call.vertexUniformData = mvp.m;
    call.vertexUniformDataSize = sizeof(ryo::Matrix4);
    call.fragmentUniformData = &col;
    call.fragmentUniformDataSize = sizeof(col);
    call.cullMode = settings ? settings->cullMode : RYCullModeBack;
    call.windingOrder = settings ? settings->windingOrder : RYWindingOrderCounterClockwise;
    call.fillMode = settings ? settings->fillMode : RYFillModeSolid;
    call.depthMode = settings ? settings->depthMode : RYDepthModeReadWrite;
    call.stencil = settings ? settings->stencil : nil;

    return [self drawEx:&call];
}

@end
