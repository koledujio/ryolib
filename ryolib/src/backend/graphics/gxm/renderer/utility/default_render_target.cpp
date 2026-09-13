#include "../private.h"

namespace ryo {
    bool _create_default_render_target(InternalRenderer* intern) {
        SceGxmRenderTargetParams renderTargetParams = {};
        renderTargetParams.flags = 0;
        renderTargetParams.width = DISPLAY_WIDTH;
        renderTargetParams.height = DISPLAY_HEIGHT;
        renderTargetParams.scenesPerFrame = 1;
        renderTargetParams.multisampleMode = SCE_GXM_MULTISAMPLE_NONE;
        renderTargetParams.multisampleLocations = 0;
        renderTargetParams.driverMemBlock = -1;

        int renderTargetCreateResult = sceGxmCreateRenderTarget(&renderTargetParams, &intern->renderTarget);
        if (renderTargetCreateResult < 0) {
            error_set("Failed to create GXM render target %d", renderTargetCreateResult);
            return false;
        }

        return true;
    }
}