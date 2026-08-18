#include "../private.h"

namespace ryo {
    ResourcePool<_InternalRenderer, MAX_RENDERERS> _renderer_pool;

    Renderer renderer_create(const RendererConfig& config) {
        if (_renderer_pool.active_count >= MAX_RENDERERS) {
            error_set("Maximum number of renderers reached");
            return {};
        }

        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            error_set("Failed to create metal device");
            return {};
        }

        Renderer result = _renderer_create(config, device);
        [device release];
        return result;
    }

    Renderer renderer_create_share_resources(const RendererConfig& config, Renderer renderer) {
        if (_renderer_pool.active_count >= MAX_RENDERERS) {
            error_set("Maximum number of renderers reached");
            return {};
        }

        id<MTLDevice> device = _renderer_pool.items[renderer.index()].device;
        if (!device) {
            error_set("Failed to create shared metal device");
            return {};
        }

        return _renderer_create(config, device);
    }

    void renderer_destroy(Renderer renderer) {
        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");
        INDEX idx = renderer.index();
        _InternalRenderer& ir = _renderer_pool.items[idx];

        if (ir.metalLayer) {
            [ir.metalLayer removeFromSuperlayer];
            ir.metalLayer = nil;
        }

        pipeline_destroy(ir.defaultColorPipeline);
        pipeline_destroy(ir.defaultTexturePipeline);
        ir.defaultColorPipeline = { 0 };
        ir.defaultTexturePipeline = { 0 };

        if (ir.depthStencil) {
            [ir.depthStencil release];
            ir.depthStencil = nil;
        }

        if (ir.mainWindowDepthTexture) {
            [ir.mainWindowDepthTexture release];
            ir.mainWindowDepthTexture = nil;
        }

        if (ir.nearestSampler) [ir.nearestSampler release];
        if (ir.linearSampler) [ir.linearSampler release];
        if (ir.minNearestMagLinearSampler) [ir.minNearestMagLinearSampler release];
        if (ir.minLinearMagNearestSampler) [ir.minLinearMagNearestSampler release];
        ir.nearestSampler = nil;
        ir.linearSampler = nil;
        ir.minNearestMagLinearSampler = nil;
        ir.minLinearMagNearestSampler = nil;

        if (ir.shaderLibrary) {
            [ir.shaderLibrary release];
            ir.shaderLibrary = nil;
        }

        if (ir.autoreleasePool) {
            [ir.autoreleasePool drain];
            ir.autoreleasePool = nil;
        }

        if (ir.queue) {
            [ir.queue release];
        }

        if (ir.device) {
            [ir.device release];
        }

        ir.queue = nil;
        ir.device = nil;
        ir.currentCommandBuffer = nil;
        ir.currentDrawable = nil;

        _renderer_pool.free_index(idx);
    }

    bool renderer_is_valid(Renderer renderer) {
        INDEX idx = renderer.index();

        if (idx >= MAX_RENDERERS)
            return false;

        if (!_renderer_pool.is_valid(idx, renderer.gen()))
            return false;

        _InternalRenderer& ir = _renderer_pool.items[idx];

        return ir.device != nil && ir.metalLayer != nil;
    }

    bool renderer_resize(Renderer renderer, int width, int height) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        if (width == ir.width && height == ir.height) {
            error_set("Failed to resize renderer, new size and current size are the same");
            return false;
        }
        
        ir.metalLayer.drawableSize = NSMakeSize(width, height);

        if (ir.mainWindowDepthTexture) {
            [ir.mainWindowDepthTexture release];
            ir.mainWindowDepthTexture = nil;
        }

        MTLTextureDescriptor* depthTextureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float width:width height:height mipmapped:NO];
        depthTextureDesc.usage = MTLTextureUsageRenderTarget;
        depthTextureDesc.storageMode = MTLStorageModePrivate;
        ir.mainWindowDepthTexture = [ir.device newTextureWithDescriptor:depthTextureDesc];

        ir.width = width;
        ir.height = height;

        return true;
    }
}
