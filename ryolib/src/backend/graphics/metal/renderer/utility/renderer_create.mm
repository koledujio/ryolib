#include "../private.h"

namespace ryo {
    Renderer _renderer_create(const RendererConfig& config, id<MTLDevice> device) {
        INDEX idx = _renderer_pool.allocate_index();
        GENID gen = _renderer_pool.generations[idx];

        id<MTLCommandQueue>queue = [device newCommandQueue];

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

        _InternalRenderer& ir = _renderer_pool.items[idx];
        ir.device = [device retain];
        ir.queue = queue;
        ir.metalLayer = layer;
        ir.width = config.width;
        ir.height = config.height;

        if (!_renderer_pipeline_init(Renderer::pack(idx, gen), ir)) {
            return {};
        }

        _renderer_sampler_init(ir);

        MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
        depthStencilDesc.depthCompareFunction = MTLCompareFunctionLess;
        depthStencilDesc.depthWriteEnabled = YES;
        ir.depthStencil = [ir.device newDepthStencilStateWithDescriptor:depthStencilDesc];
        [depthStencilDesc release];

        MTLTextureDescriptor* depthTextureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float width:config.width height:config.height mipmapped:NO];
        depthTextureDesc.usage = MTLTextureUsageRenderTarget;
        depthTextureDesc.storageMode = MTLStorageModePrivate;
        ir.mainWindowDepthTexture = [device newTextureWithDescriptor:depthTextureDesc];

        return Renderer::pack(idx, gen);
    }
}