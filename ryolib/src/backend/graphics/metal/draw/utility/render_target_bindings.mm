#include "../private.h"

namespace ryo {
    id<MTLTexture> _draw_get_color_texture(_InternalRenderer& ir, RenderTarget* target) {
        if (target != nullptr) {
            return _render_target_pool.items[target->index()].colorTexture;
        }

        return ir.currentDrawable.texture;
    }

    id<MTLTexture> _draw_get_depth_texture(_InternalRenderer& ir, RenderTarget* target) {
        if (target != nullptr) {
            return _render_target_pool.items[target->index()].depthTexture;
        }

        return ir.mainWindowDepthTexture;
    }
}