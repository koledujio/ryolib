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

        MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
        depthStencilDesc.depthCompareFunction = MTLCompareFunctionLess;
        depthStencilDesc.depthWriteEnabled = YES;
        intern->depthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilDesc];
        [depthStencilDesc release];

        MTLTextureDescriptor* depthTextureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float width:config.width height:config.height mipmapped:NO];
        depthTextureDesc.usage = MTLTextureUsageRenderTarget;
        depthTextureDesc.storageMode = MTLStorageModePrivate;
        intern->mainWindowDepthTexture = [device newTextureWithDescriptor:depthTextureDesc];

        if (!_renderer_pipeline_init(renderer, *intern)) {
            [intern->metalLayer release];
            [intern->depthStencilState release];
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