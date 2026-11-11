#include "../private.h"

namespace ryo {
    id<MTLTexture> _draw_get_color_texture(InternalRenderer& ir, RenderTarget* target) {
        // what the fuck happened here
        if (target != nullptr) {
            if (target->internal != nullptr) {
                if (target->internal->colorTexture != nullptr) {
                    if (target->internal->colorTexture->internal != nullptr) {
                        if (target->internal->colorTexture->internal->texture != nil) {
                            return target->internal->colorTexture->internal->texture;
                        }
                    }
                }
            }
        }

        return ir.currentDrawable.texture;
    }

    id<MTLTexture> _draw_get_depth_texture(InternalRenderer& ir, RenderTarget* target) {
        if (target != nullptr) {
            if (target->internal != nullptr) {
                if (target->internal->depthTexture != nullptr) {
                    if (target->internal->depthTexture->internal != nullptr) {
                        if (target->internal->depthTexture->internal->texture != nil) {
                            return target->internal->depthTexture->internal->texture;
                        }
                    }
                }
            }
        }

        return ir.mainWindowDepthTexture;
    }
}