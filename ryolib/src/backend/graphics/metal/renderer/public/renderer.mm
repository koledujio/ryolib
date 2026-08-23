#include "../private.h"

namespace ryo {
    Renderer::Renderer(const RendererConfig& config) {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            error_set("Failed to create metal device");
            return;
        }

        _renderer_create(*this, config, device);

        [device release];
    }

    Renderer::Renderer(const RendererConfig& config, const Renderer& renderer) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");

        id<MTLDevice> device = renderer.internal->device;
        if (!device) {
            error_set("Failed to create shared metal device");
            return;
        }

        _renderer_create(*this, config, device);
    }

    Renderer::~Renderer() {
        if (!internal)
            return;

        if (internal->metalLayer)
            [internal->metalLayer removeFromSuperlayer];

        if (internal->defaultColorPipeline)
            delete internal->defaultColorPipeline;
        
        if (internal->defaultTexturePipeline)
            delete internal->defaultTexturePipeline;

        if (internal->depthStencilState)
            [internal->depthStencilState release];

        if (internal->mainWindowDepthTexture)
            [internal->mainWindowDepthTexture release];

        if (internal->nearestSampler)
            [internal->nearestSampler release];

        if (internal->linearSampler)
            [internal->linearSampler release];

        if (internal->minNearestMagLinearSampler)
            [internal->minNearestMagLinearSampler release];

        if (internal->minLinearMagNearestSampler)
            [internal->minLinearMagNearestSampler release];

        if (internal->shaderLibrary)
            [internal->shaderLibrary release];

        if (internal->autoreleasePool)
            [internal->autoreleasePool drain];

        if (internal->queue)
            [internal->queue release];

        if (internal->device)
            [internal->device release];

        delete internal;
    }

    bool Renderer::valid() const {
        // this could probably be improved...
        return (internal != nullptr &&
                internal->device != nil &&
                internal->queue != nil &&
                internal->shaderLibrary != nil &&
                // internal->defaultColorPipeline != nullptr &&
                // internal->defaultColorPipeline->valid() &&
                // internal->defaultTexturePipeline != nullptr &&
                // internal->defaultTexturePipeline->valid() &&
                internal->depthStencilState != nil &&
                internal->mainWindowDepthTexture != nil &&
                internal->nearestSampler != nil &&
                internal->linearSampler != nil &&
                internal->minNearestMagLinearSampler != nil &&
                internal->minLinearMagNearestSampler != nil &&
                internal->metalLayer != nullptr);
    }

    bool Renderer::resize(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        if (width == internal->width && height == internal->height) {
            error_set("Failed to resize renderer, new size and current size are the same");
            return false;
        }
        
        internal->metalLayer.drawableSize = NSMakeSize(width, height);

        if (internal->mainWindowDepthTexture) {
            [internal->mainWindowDepthTexture release];
            internal->mainWindowDepthTexture = nil;
        }

        MTLTextureDescriptor* depthTextureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float width:width height:height mipmapped:NO];
        depthTextureDesc.usage = MTLTextureUsageRenderTarget;
        depthTextureDesc.storageMode = MTLStorageModePrivate;
        internal->mainWindowDepthTexture = [internal->device newTextureWithDescriptor:depthTextureDesc];

        internal->width = width;
        internal->height = height;

        return true;
    }
}
