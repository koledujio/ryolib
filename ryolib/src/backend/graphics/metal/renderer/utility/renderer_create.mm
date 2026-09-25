#include "../private.h"

namespace ryo {
    bool _renderer_create(Renderer& renderer, const RendererConfig& config, id<MTLDevice> device) {
        id<MTLCommandQueue> queue = [device newCommandQueue];

        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.device = device;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.framebufferOnly = YES;
        layer.drawableSize = CGSizeMake(config.width, config.height);

        if (config.nativeWindowHandle) {
            NSWindow* window = (__bridge NSWindow*)config.nativeWindowHandle;
            if (window && window.contentView) {
                window.contentView.layer = layer;
                window.contentView.wantsLayer = YES;
            }
        }

        InternalRenderer* intern = new InternalRenderer;
        intern->device = [device retain];
        intern->queue = queue;
        intern->metalLayer = layer;
        intern->width = config.width;
        intern->height = config.height;

        renderer.internal = intern;

        _renderer_sampler_init(*intern);

        // DISABLED, READ, READ_WRITE
        MTLCompareFunction depthFuncs[3] = { MTLCompareFunctionAlways, MTLCompareFunctionLessEqual, MTLCompareFunctionLessEqual };
        BOOL depthWrites[3] = { NO, NO, YES };

        for (int i = 0; i < 3; i++) {
            MTLDepthStencilDescriptor* desc = [[MTLDepthStencilDescriptor alloc] init];
            desc.depthCompareFunction = depthFuncs[i];
            desc.depthWriteEnabled = depthWrites[i];
            intern->depthStates[i] = [device newDepthStencilStateWithDescriptor:desc];
            [desc release];
        }

        MTLTextureDescriptor* depthTextureDesc =
            [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8
                                  width:config.width
                                  height:config.height
                                  mipmapped:NO];
        
        depthTextureDesc.usage = MTLTextureUsageRenderTarget;
        depthTextureDesc.storageMode = MTLStorageModePrivate;
        intern->mainWindowDepthTexture = [device newTextureWithDescriptor:depthTextureDesc];

        MTLDepthStencilDescriptor* stencilWriteDesc = [[MTLDepthStencilDescriptor alloc] init];
        stencilWriteDesc.depthCompareFunction = MTLCompareFunctionAlways;
        stencilWriteDesc.depthWriteEnabled = NO;
        stencilWriteDesc.frontFaceStencil.stencilCompareFunction = MTLCompareFunctionAlways;
        stencilWriteDesc.frontFaceStencil.stencilFailureOperation = MTLStencilOperationKeep;
        stencilWriteDesc.frontFaceStencil.depthFailureOperation = MTLStencilOperationKeep;
        stencilWriteDesc.frontFaceStencil.depthStencilPassOperation = MTLStencilOperationReplace;
        stencilWriteDesc.frontFaceStencil.readMask = 0xFF;
        stencilWriteDesc.frontFaceStencil.writeMask = 0xFF;
        stencilWriteDesc.backFaceStencil = stencilWriteDesc.frontFaceStencil;
        intern->stencilWriteState = [device newDepthStencilStateWithDescriptor:stencilWriteDesc];
        [stencilWriteDesc release];

        for (int i = 0; i < 3; i++) {
            MTLDepthStencilDescriptor* desc = [[MTLDepthStencilDescriptor alloc] init];
            desc.depthCompareFunction = depthFuncs[i];
            desc.depthWriteEnabled = depthWrites[i];
            desc.frontFaceStencil.stencilCompareFunction = MTLCompareFunctionEqual;
            desc.frontFaceStencil.stencilFailureOperation = MTLStencilOperationKeep;
            desc.frontFaceStencil.depthFailureOperation = MTLStencilOperationKeep;
            desc.frontFaceStencil.depthStencilPassOperation = MTLStencilOperationKeep;
            desc.frontFaceStencil.readMask = 0xFF;
            desc.frontFaceStencil.writeMask = 0x00;
            desc.backFaceStencil = desc.frontFaceStencil;
            intern->stencilTestStates[i] = [device newDepthStencilStateWithDescriptor:desc];
            [desc release];
        }

        if (!_renderer_pipeline_init(renderer, *intern) || !_renderer_buffers_init(renderer, *intern)) {
            [intern->metalLayer release];
            for (int i = 0; i < 3; i++) {
                if (intern->depthStates[i]) [intern->depthStates[i] release];
                if (intern->stencilTestStates[i]) [intern->stencilTestStates[i] release];
            }
            if (intern->stencilWriteState) [intern->stencilWriteState release];
            [intern->mainWindowDepthTexture release];
            [intern->nearestSampler release];
            [intern->linearSampler release];
            [intern->minNearestMagLinearSampler release];
            [intern->minLinearMagNearestSampler release];
            [intern->queue release];
            [intern->device release];
            delete intern;
            return false;
        }

        return true;
    }
}