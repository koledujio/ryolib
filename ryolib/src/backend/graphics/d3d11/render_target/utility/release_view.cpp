#include "../private.h"

namespace ryo {
    void _render_target_release_views(InternalRenderTarget& renderTarget) {
        if (renderTarget.colorView) {
            renderTarget.colorView->Release();
            renderTarget.colorView = nullptr;
        }

        if (renderTarget.depthView) {
            renderTarget.depthView->Release();
            renderTarget.depthView = nullptr;
        }
    }
}