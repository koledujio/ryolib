#include "../private.h"

namespace ryo {
    void draw_pass_begin(Renderer renderer, RenderTarget* target, const Color* clearColor) {
        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];
        if (!ir.currentCommandBuffer || !ir.currentDrawable) return;

        RYO_ENSURE_VOID(!ir.inPass, "Renderer already in a pass");

        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = _draw_get_color_texture(ir, target);
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.depthAttachment.texture = _draw_get_depth_texture(ir, target);
        pass.depthAttachment.storeAction = MTLStoreActionStore;

        if (clearColor != nullptr) {
            pass.colorAttachments[0].loadAction = MTLLoadActionClear;
            pass.colorAttachments[0].clearColor = MTLClearColorMake(clearColor->r, clearColor->g, clearColor->b, clearColor->a);
            pass.depthAttachment.loadAction = MTLLoadActionClear;
            pass.depthAttachment.clearDepth = 1.0f;
        } else {
            pass.colorAttachments[0].loadAction = MTLLoadActionLoad;
            pass.depthAttachment.loadAction = MTLLoadActionLoad;
        }

        ir.currentEncoder = [ir.currentCommandBuffer renderCommandEncoderWithDescriptor:pass];
        ir.currentTarget = target;
        ir.inPass = true;
    }

    void draw_pass_end(Renderer renderer) {
        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        RYO_ENSURE_VOID(ir.inPass || !ir.currentEncoder, "Renderer already in a pass or no current encoder");

        [ir.currentEncoder endEncoding];
        ir.currentEncoder = nil;
        ir.currentTarget = nullptr;
        ir.inPass = false;
    }
}