#pragma once

#include "../private.h"

#include "ryolib/graphics.h"

#import "RYMetalGraphicsBackend.h"

#include <CoreFoundation/CFDate.h>
#include <cmath>
#include <math.h>

#include <vector>
#include <string>

#include <CoreGraphics/CGGeometry.h>
#include <cstddef>
#include <objc/objc.h>

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#if defined(RYO_APPLE_IOS) || TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif
#import <simd/simd.h>

#include "ryolib/templates.h"

struct RYInternalRenderer {
    id<MTLDevice> device = nil;
    id<MTLCommandQueue> queue = nil;
    int width = 0;
    int height = 0;
    float contentScale = 1.0f;

    id<MTLLibrary> shaderLibrary = nil;
    RYPipeline *defaultColorPipeline = nil;
    RYPipeline *defaultTexturePipeline = nil;
    RYPipeline *stencilPipeline = nil;

    RYBuffer *quadVertexBuffer = nil;
    RYBuffer *quadIndexBuffer = nil;
    RYBuffer *quadVertexBufferUV = nil;
    RYBuffer *quadIndexBufferUV = nil;

    id<MTLDepthStencilState> depthStates[3] = {nil, nil, nil};
    id<MTLDepthStencilState> stencilTestStates[3] = {nil, nil, nil};
    id<MTLDepthStencilState> stencilWriteState = nil;
    id<MTLTexture> mainWindowDepthTexture = nil;

    id<MTLSamplerState> nearestSampler = nil;
    id<MTLSamplerState> linearSampler = nil;
    id<MTLSamplerState> minNearestMagLinearSampler = nil;
    id<MTLSamplerState> minLinearMagNearestSampler = nil;

    CAMetalLayer *metalLayer = nullptr;
    id<CAMetalDrawable> currentDrawable = nil;

    id<MTLCommandBuffer> currentCommandBuffer = nil;
    NSAutoreleasePool *autoreleasePool = nil;

    id<MTLRenderCommandEncoder> currentEncoder = nil;
    RYRenderTarget *currentTarget = nil;
    bool inPass = false;

    bool inFrame = false;

    float deltaTime = 0.0f;
    CFTimeInterval lastTime = 0.0;
};

struct RYInternalRenderTarget {
    int width = 0;
    int height = 0;
};

struct RYInternalTexture {
    id<MTLTexture> texture = nil;
};

struct RYInternalBuffer {
    id<MTLBuffer> buffer = nil;
};

struct RYInternalPipeline {
    id<MTLRenderPipelineState> pso = nil;
};

enum RYStencilShapeType {
    _RY_STENCIL_SHAPE_RECTANGLE,
    _RY_STENCIL_SHAPE_ROUNDED_RECTANGLE,
    _RY_STENCIL_SHAPE_OVAL
};

struct RYStencilCommand {
    RYStencilShapeType shape;
    ryo::Matrix4 matrix;

    float radius; // rounded rectangle requires this
    RYCamera camera;
    bool hasCamera;
};

struct RYStencilVertexUniforms {
    ryo::Matrix4 matrix;
};

struct RYStencilFragmentUniforms {
    ryo::Vector2 scale;
    float radius;

    uint32_t shape;
};

struct RYInternalStencil {
    RYInternalRenderer *renderer = nullptr;
    RYInternalRenderTarget *renderTarget = nullptr;

    RYStencilCommand *commands = nullptr;
    size_t commandCount = 0;
    size_t commandCapacity = 0;
};

ryo::Matrix4 _compute_camera_vp(const RYCamera &camera, float aspect);
void _draw_get_target_size(const RYInternalRenderer &renderer, int &width, int &height);
void _renderer_set_layer_size(RYInternalRenderer &intern, int width, int height);
void _renderer_sync_layer_size(RYInternalRenderer &intern);
