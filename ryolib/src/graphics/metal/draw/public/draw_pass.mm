#include "../private.h"

@implementation RYMetalGraphicsBackend (DrawPass)

- (void)drawPassBeginWithRenderTarget:(RYRenderTarget *)target clearColor:(const RYColor *)clearColor {
    if (!_internal) return;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (!intern->inFrame || intern->inPass) return;

    MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
    pass.colorAttachments[0].texture = _draw_get_color_texture(*intern, target);
    pass.colorAttachments[0].storeAction = MTLStoreActionStore;
    pass.depthAttachment.texture = _draw_get_depth_texture(*intern, target);
    pass.depthAttachment.storeAction = MTLStoreActionStore;
    pass.stencilAttachment.texture = _draw_get_depth_texture(*intern, target);
    pass.stencilAttachment.storeAction = MTLStoreActionStore;

    if (clearColor != nullptr) {
        pass.colorAttachments[0].loadAction = MTLLoadActionClear;
        pass.colorAttachments[0].clearColor = MTLClearColorMake(clearColor->r, clearColor->g, clearColor->b, clearColor->a);
        pass.depthAttachment.loadAction = MTLLoadActionClear;
        pass.depthAttachment.clearDepth = 1.0f;
        pass.stencilAttachment.loadAction = MTLLoadActionClear;
        pass.stencilAttachment.clearStencil = 0;
    } else {
        pass.colorAttachments[0].loadAction = MTLLoadActionLoad;
        pass.depthAttachment.loadAction = MTLLoadActionLoad;
        pass.stencilAttachment.loadAction = MTLLoadActionLoad;
    }

    intern->currentEncoder = [intern->currentCommandBuffer renderCommandEncoderWithDescriptor:pass];
    intern->currentTarget = target;
    intern->inPass = true;
}

- (void)drawPassEnd {
    if (!_internal) return;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (!intern->inFrame || !intern->inPass) return;

    [intern->currentEncoder endEncoding];
    intern->currentEncoder = nil;
    intern->currentTarget = nil;
    intern->inPass = false;
}

@end
