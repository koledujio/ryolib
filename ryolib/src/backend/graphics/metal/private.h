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
    constexpr uint32_t MAX_RENDERERS = 8;
    constexpr uint32_t MAX_TEXTURES = 1024;
    constexpr uint32_t MAX_BUFFERS = 1024;
    constexpr uint32_t MAX_RENDER_TARGETS = 256;
    constexpr uint32_t MAX_PIPELINES = 512;

    struct _InternalRenderer {
        id<MTLDevice> device = nil;
        id<MTLCommandQueue> queue = nil;
        int width = 0;
        int height = 0;

        id<MTLLibrary> shaderLibrary = nil;
        Pipeline defaultColorPipeline = { 0 };
        Pipeline defaultTexturePipeline = { 0 };
        
        id<MTLDepthStencilState> depthStencil = nil;
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

        float deltaTime = 0.0f;
        CFTimeInterval lastTime = 0.0;
    };

    struct _InternalRenderTarget {
        id<MTLTexture> colorTexture = nil;
        id<MTLTexture> depthTexture = nil;
        int width = 0;
        int height = 0;
        uint32_t colorTextureId = 0;
        uint32_t depthTextureId = 0; // todo replace this uint32_t with a Texture, since it's basically a glorified uint32_t id, but type safe for only Texture related stuff
        // Matrix4 viewProjectionMatrix;
    };

    extern ResourcePool<_InternalRenderer, MAX_RENDERERS> _renderer_pool;

    extern ResourcePool<id<MTLTexture>, MAX_TEXTURES> _texture_pool;

    extern ResourcePool<id<MTLBuffer>, MAX_BUFFERS> _buffer_pool;

    extern ResourcePool<_InternalRenderTarget, MAX_RENDER_TARGETS> _render_target_pool;

    extern ResourcePool<id<MTLRenderPipelineState>, MAX_PIPELINES> _pipeline_pool;
}
