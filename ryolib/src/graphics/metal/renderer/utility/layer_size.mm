#include "../../private.h"

void _renderer_set_layer_size(RYInternalRenderer &intern, int width, int height) {
    intern.metalLayer.drawableSize = CGSizeMake(width, height);

    if (intern.mainWindowDepthTexture) {
        [intern.mainWindowDepthTexture release];
        intern.mainWindowDepthTexture = nil;
    }

    MTLTextureDescriptor *depthTextureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8 width:width height:height mipmapped:NO];
    depthTextureDesc.usage = MTLTextureUsageRenderTarget;
    depthTextureDesc.storageMode = MTLStorageModePrivate;
    intern.mainWindowDepthTexture = [intern.device newTextureWithDescriptor:depthTextureDesc];

    intern.width = width;
    intern.height = height;
}

void _renderer_sync_layer_size(RYInternalRenderer &intern) {
    CAMetalLayer *layer = intern.metalLayer;

#if defined(RYO_APPLE_IOS) || TARGET_OS_IPHONE
    UIView *view = (UIView *)layer.delegate;
#else
    NSView *view = (NSView *)layer.delegate;
#endif
    CGRect viewBounds = view ? view.bounds : layer.bounds;

    if (!view && layer.superlayer && !CGRectEqualToRect(layer.frame, viewBounds))
        layer.frame = viewBounds;

    float contentScale = intern.contentScale > 0.0f ? intern.contentScale : 1.0f;

    if (layer.contentsScale != contentScale)
        layer.contentsScale = contentScale;

    int width = (int)(viewBounds.size.width * contentScale);
    int height = (int)(viewBounds.size.height * contentScale);

    if (width < 1 || height < 1)
        return;

    if (width == intern.width && height == intern.height)
        return;

    _renderer_set_layer_size(intern, width, height);
}
