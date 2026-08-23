#include "../private.h"

namespace ryo {
    void Renderer::draw_pass_begin(RenderTarget* target, const Color* clearColor) {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(!internal->inPass, "Renderer already in a pass");

        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = _draw_get_color_texture(*internal, target);
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.depthAttachment.texture = _draw_get_depth_texture(*internal, target);
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

        internal->currentEncoder = [internal->currentCommandBuffer renderCommandEncoderWithDescriptor:pass];
        internal->currentTarget = target;
        internal->inPass = true;
    }

    void Renderer::draw_pass_end() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer handle");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(internal->inPass, "Renderer not in a pass");

        [internal->currentEncoder endEncoding];
        internal->currentEncoder = nil;
        internal->currentTarget = nullptr;
        internal->inPass = false;
    }
}