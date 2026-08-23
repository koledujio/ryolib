#include "../private.h"

namespace ryo {
    void _render_target_release(InternalRenderTarget& renderTarget) {
        _render_target_release_views(renderTarget);

        if (renderTarget.colorTexture->valid()) {
            delete renderTarget.colorTexture;
        }

        if (renderTarget.depthTexture->valid()) {
            delete renderTarget.depthTexture;
        }
    }
}