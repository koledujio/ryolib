#include "../private.h"

namespace ryo {
    void _render_target_release(_InternalRenderTarget& renderTarget) {
        _render_target_release_views(renderTarget);

        if (texture_is_valid(renderTarget.colorTextureHandle)) {
            texture_destroy(renderTarget.colorTextureHandle);
        }

        if (texture_is_valid(renderTarget.depthTextureHandle)) {
            texture_destroy(renderTarget.depthTextureHandle);
        }

        renderTarget = {};
    }
}