#include "../private.h"

namespace ryo {
    void _draw_apply_depth_settings(SceGxmContext* context, const DrawSettings* settings) {
        // TODO: enable an option to disable depth/customize depth settings in DrawSettings
        sceGxmSetFrontDepthFunc(context, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
        sceGxmSetBackDepthFunc(context, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
        sceGxmSetFrontDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_ENABLED);
        sceGxmSetBackDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_ENABLED);
    }
}