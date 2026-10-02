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
        
        if (internal->stencilPipeline)
            delete internal->stencilPipeline;

        if (internal->quadVertexBuffer)
            delete internal->quadVertexBuffer;

        if (internal->quadIndexBuffer)
            delete internal->quadIndexBuffer;

        if (internal->quadVertexBufferUV)
            delete internal->quadVertexBufferUV;

        if (internal->quadIndexBufferUV)
            delete internal->quadIndexBufferUV;

        for (int i = 0; i < 3; i++) {
            if (internal->depthStates[i])
                [internal->depthStates[i] release];
            if (internal->stencilTestStates[i])
                [internal->stencilTestStates[i] release];
        }

        if (internal->stencilWriteState)
            [internal->stencilWriteState release];

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
        return (internal != nullptr &&
                internal->device != nil &&
                internal->queue != nil &&
                internal->shaderLibrary != nil &&
                internal->depthStates[0] != nil &&
                internal->mainWindowDepthTexture != nil &&
                internal->nearestSampler != nil &&
                internal->linearSampler != nil &&
                internal->minNearestMagLinearSampler != nil &&
                internal->minLinearMagNearestSampler != nil &&
                internal->metalLayer != nullptr);
    }

    bool Renderer::resize(unsigned int width, unsigned int height) {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        // width and height are logical, the framebuffer is rendered at the content scale
        unsigned int pixelWidth = static_cast<unsigned int>(static_cast<float>(width) * internal->contentScale);
        unsigned int pixelHeight = static_cast<unsigned int>(static_cast<float>(height) * internal->contentScale);

        if (pixelWidth == static_cast<unsigned int>(internal->width) &&
            pixelHeight == static_cast<unsigned int>(internal->height)) {
            error_set("Failed to resize renderer, new size and current size are the same");
            return false;
        }

        _renderer_set_layer_size(*internal, pixelWidth, pixelHeight);

        return true;
    }

    bool Renderer::size_get(unsigned int& outWidth, unsigned int& outHeight) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        outWidth = static_cast<unsigned int>(internal->width);
        outHeight = static_cast<unsigned int>(internal->height);

        return true;
    }
}
