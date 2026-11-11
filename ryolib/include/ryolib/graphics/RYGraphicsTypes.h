#pragma once

#include <stdint.h>
#include <stddef.h>
#import "../objc/RYObject.h"
#import "../objc/RYColor.h"
#include "../math.h"

@class RYRenderer;
@class RYTexture;
@class RYBuffer;
@class RYPipeline;
@class RYRenderTarget;
@class RYStencil;

/**
 * @brief Configuration of a renderer's framebuffer and graphics device.
 */
typedef struct RYRendererConfig {
    void *nativeWindowHandle;
    int width;
    int height;
    float contentScale;
} RYRendererConfig;

/**
 * @brief Vertex with a position.
 */
typedef struct RYPositionVertex {
    float position[3];
} RYPositionVertex;

/**
 * @brief Vertex with a position and texture coordinates.
 */
typedef struct RYPositionUVVertex {
    float position[3];
    float uv[2];
} RYPositionUVVertex;

/**
 * @brief Camera projection types.
 */
typedef enum {
    RYCameraProjectionTypeOrthographic,
    RYCameraProjectionTypePerspective
} RYCameraProjectionType;

/**
 * @brief Camera definition.
 */
typedef struct RYCamera {
    ryo::Vector3 position;
    ryo::Vector3 target;
    ryo::Vector3 up;
    RYCameraProjectionType projectionType;
    float fovY;
    float orthographicSize;
    float nearPlane;
    float farPlane;
} RYCamera;

/**
 * @brief Texture sampler filtering types.
 */
typedef enum {
    RYSamplerTypeNearest,
    RYSamplerTypeLinear,
    RYSamplerTypeMinNearestMagLinear,
    RYSamplerTypeMinLinearMagNearest
} RYSamplerType;

/**
 * @brief Buffer types.
 */
typedef enum {
    RYBufferTypeVertex,
    RYBufferTypeIndex
} RYBufferType;

/**
 * @brief Vertex layouts.
 */
typedef enum {
    RYVertexLayoutTypePosition,
    RYVertexLayoutTypePositionUV
} RYVertexLayoutType;

/**
 * @brief Blend modes.
 */
typedef enum {
    RYBlendModeOpaque,
    RYBlendModeAlpha,
    RYBlendModeAdditive
} RYBlendMode;

/**
 * @brief Built-in shaders.
 */
typedef enum {
    RYBuiltinShaderTexture,
    RYBuiltinShaderColor,
    RYBuiltinShaderStencil
} RYBuiltinShader;

/**
 * @brief Built-in pipelines.
 */
typedef enum {
    RYBuiltinPipelineColor,
    RYBuiltinPipelineTexture,
    RYBuiltinPipelineStencil
} RYBuiltinPipeline;

/**
 * @brief Pipeline description.
 */
typedef struct RYPipelineDesc {
    RYBuiltinShader shader;
    RYVertexLayoutType vertexLayout;
    RYBlendMode blendMode;
} RYPipelineDesc;

/**
 * @brief Face culling modes.
 */
typedef enum {
    RYCullModeNone,
    RYCullModeFront,
    RYCullModeBack
} RYCullMode;

/**
 * @brief Triangle winding orders.
 */
typedef enum {
    RYWindingOrderClockwise,
    RYWindingOrderCounterClockwise
} RYWindingOrder;

/**
 * @brief Triangle fill modes.
 */
typedef enum {
    RYFillModeSolid,
    RYFillModeWireframe
} RYFillMode;

/**
 * @brief Depth modes.
 */
typedef enum {
    RYDepthModeDisabled,
    RYDepthModeRead,
    RYDepthModeReadWrite
} RYDepthMode;

/**
 * @brief Draw settings and overrides.
 */
typedef struct RYDrawSettings {
    RYCullMode cullMode;
    RYWindingOrder windingOrder;
    RYFillMode fillMode;
    RYDepthMode depthMode;
    const RYCamera *camera;
    RYPipeline *pipelineOverride;
    RYStencil *stencil;
} RYDrawSettings;

static inline RYDrawSettings RYDrawSettingsMakeDefault(void) {
    RYDrawSettings s;
    s.cullMode = RYCullModeBack;
    s.windingOrder = RYWindingOrderCounterClockwise;
    s.fillMode = RYFillModeSolid;
    s.depthMode = RYDepthModeReadWrite;
    s.camera = NULL;
    s.pipelineOverride = nil;
    s.stencil = nil;
    return s;
}

/**
 * @brief Low level draw call submission.
 */
typedef struct RYDrawCall {
    RYPipeline *pipeline;
    RYBuffer *vertexBuffer;
    uint32_t vertexStride;
    RYBuffer *indexBuffer;
    uint32_t indexCount;
    RYTexture **textures;
    RYSamplerType *samplerTypes;
    uint32_t textureCount;
    const void *vertexUniformData;
    size_t vertexUniformDataSize;
    const void *fragmentUniformData;
    size_t fragmentUniformDataSize;
    RYCullMode cullMode;
    RYWindingOrder windingOrder;
    RYFillMode fillMode;
    RYDepthMode depthMode;
    RYStencil *stencil;
} RYDrawCall;
