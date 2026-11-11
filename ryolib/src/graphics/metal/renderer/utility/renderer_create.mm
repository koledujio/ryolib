#include "../private.h"

RYInternalRenderer* _renderer_create(const RYRendererConfig &config, id<MTLDevice> device, RYError **error) {
    id<MTLCommandQueue> queue = [device newCommandQueue];

    float contentScale = config.contentScale;
    if (contentScale <= 0.0f)
        contentScale = 1.0f;

    // render resolution = logical size * content scale
    int width = static_cast<int>(static_cast<float>(config.width) * contentScale);
    int height = static_cast<int>(static_cast<float>(config.height) * contentScale);

    CAMetalLayer *layer = [CAMetalLayer layer];
#if defined(RYO_APPLE_IOS) || TARGET_OS_IPHONE
    if (config.nativeWindowHandle) {
        UIView *view = (__bridge UIView *)config.nativeWindowHandle;
        CAMetalLayer *sdlLayer = (CAMetalLayer *)view.layer;
        if (view && sdlLayer) {
            CGFloat scale = sdlLayer.contentsScale;
            if (scale <= 0.0)
                scale = 1.0;

            CGRect viewBounds = view.bounds;
            width = static_cast<int>(viewBounds.size.width * scale);
            height = static_cast<int>(viewBounds.size.height * scale);
            contentScale = static_cast<float>(scale);
            layer = sdlLayer;
        }
    }
#else
    if (config.nativeWindowHandle) {
        NSWindow *window = (__bridge NSWindow *)config.nativeWindowHandle;
        if (window && window.contentView) {
            window.contentView.wantsLayer = YES;
            window.contentView.layer = layer;
            layer.frame = window.contentView.bounds;
            layer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
        }
    }
#endif

    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    layer.framebufferOnly = YES;
    layer.contentsScale = contentScale;
    layer.drawableSize = CGSizeMake(width, height);

    RYInternalRenderer *intern = new RYInternalRenderer;
    intern->device = [device retain];
    intern->queue = queue;
    intern->metalLayer = [layer retain];
    intern->contentScale = contentScale;

    _renderer_sampler_init(*intern);

    // drawable and depth texture at the render resolution
    _renderer_set_layer_size(*intern, width, height);

    // DISABLED, READ, READ_WRITE
    MTLCompareFunction depthFuncs[3] = { MTLCompareFunctionAlways, MTLCompareFunctionLessEqual, MTLCompareFunctionLessEqual };
    BOOL depthWrites[3] = { NO, NO, YES };

    for (int i = 0; i < 3; i++) {
        MTLDepthStencilDescriptor *desc = [[MTLDepthStencilDescriptor alloc] init];
        desc.depthCompareFunction = depthFuncs[i];
        desc.depthWriteEnabled = depthWrites[i];
        intern->depthStates[i] = [device newDepthStencilStateWithDescriptor:desc];
        [desc release];
    }

    MTLDepthStencilDescriptor *stencilWriteDesc = [[MTLDepthStencilDescriptor alloc] init];
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
        MTLDepthStencilDescriptor *desc = [[MTLDepthStencilDescriptor alloc] init];
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

    return intern;
}
