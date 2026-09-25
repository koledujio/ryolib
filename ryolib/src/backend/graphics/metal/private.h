#pragma once

#include "../private.h"

#include "ryolib/graphics.h"

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
#import <AppKit/AppKit.h>
#import <simd/simd.h>

#include "ryolib/templates.h"

namespace ryo {
    struct InternalRenderer {
        id<MTLDevice> device = nil;
        id<MTLCommandQueue> queue = nil;
        int width = 0;
        int height = 0;

        id<MTLLibrary> shaderLibrary = nil;
        Pipeline* defaultColorPipeline = nullptr;
        Pipeline* defaultTexturePipeline = nullptr;
        Pipeline* stencilPipeline = nullptr;

        Buffer* quadVertexBuffer = nullptr;
        Buffer* quadIndexBuffer = nullptr;
        Buffer* quadVertexBufferUV = nullptr;
        Buffer* quadIndexBufferUV = nullptr;
        
        id<MTLDepthStencilState> depthStates[3] = {nil, nil, nil};
        id<MTLDepthStencilState> stencilTestStates[3] = {nil, nil, nil};
        id<MTLDepthStencilState> stencilWriteState = nil;
        id<MTLTexture> mainWindowDepthTexture = nil;

        id<MTLSamplerState> nearestSampler = nil;
        id<MTLSamplerState> linearSampler = nil;
        id<MTLSamplerState> minNearestMagLinearSampler = nil;
        id<MTLSamplerState> minLinearMagNearestSampler = nil;

        CAMetalLayer* metalLayer = nullptr;
        id<CAMetalDrawable> currentDrawable = nil;

        id<MTLCommandBuffer> currentCommandBuffer = nil;
        NSAutoreleasePool* autoreleasePool = nil;

        id<MTLRenderCommandEncoder> currentEncoder = nil;
        RenderTarget* currentTarget = nullptr;
        bool inPass = false;

        bool inFrame = false;

        float deltaTime = 0.0f;
        CFTimeInterval lastTime = 0.0;
    };

    struct InternalRenderTarget {
        Texture* colorTexture = nullptr;
        Texture* depthTexture = nullptr;
        int width = 0;
        int height = 0;
    };

    struct InternalTexture {
        id<MTLTexture> texture = nil;
    };

    struct InternalBuffer {
        id<MTLBuffer> buffer = nil;
    };

    struct InternalPipeline {
        id<MTLRenderPipelineState> pso = nil;
    };

    enum StencilShapeType {
        _STENCIL_SHAPE_RECTANGLE,
        _STENCIL_SHAPE_ROUNDED_RECTANGLE,
        _STENCIL_SHAPE_OVAL
    };

    struct StencilCommand {
        StencilShapeType shape;
        Matrix4 matrix;

        float radius; // rounded rectangle requires this
        Camera camera;
        bool hasCamera;
    };

    struct StencilVertexUniforms {
        Matrix4 matrix;
    };

    struct StencilFragmentUniforms {
        Vector2 scale;
        float radius;

        uint32_t shape;
    };

    struct InternalStencil {
        InternalRenderer* renderer = nullptr;
        InternalRenderTarget* renderTarget = nullptr;

        StencilCommand* commands = nullptr;
        size_t commandCount = 0;
        size_t commandCapacity = 0;
    };

    Matrix4 _compute_camera_vp(const Camera& camera, float aspect);
    void _draw_get_target_size(const InternalRenderer& renderer, int& width, int& height);
}
