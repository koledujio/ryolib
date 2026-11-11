#include "../private.h"

id<MTLTexture> _draw_get_color_texture(RYInternalRenderer& ir, RYRenderTarget* target) {
    if (target != nil) {
        RYTexture *tex = target.colorTexture;
        if (tex != nil) {
            RYInternalTexture *texIntern = (RYInternalTexture *)tex.internalTexture;
            if (texIntern && texIntern->texture != nil) {
                return texIntern->texture;
            }
        }
    }

    return ir.currentDrawable.texture;
}

id<MTLTexture> _draw_get_depth_texture(RYInternalRenderer& ir, RYRenderTarget* target) {
    if (target != nil) {
        RYTexture *tex = target.depthTexture;
        if (tex != nil) {
            RYInternalTexture *texIntern = (RYInternalTexture *)tex.internalTexture;
            if (texIntern && texIntern->texture != nil) {
                return texIntern->texture;
            }
        }
    }

    return ir.mainWindowDepthTexture;
}
