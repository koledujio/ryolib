#include "../private.h"

namespace ryo {
    void _draw_apply_settings(SceGxmContext* context, const DrawSettings* settings) {
        if (!settings) {
            DrawSettings defaults = {};
            _draw_apply_settings(context, &defaults);
            return;
        }

        if (settings->cullMode == CULL_MODE_NONE) {
            sceGxmSetCullMode(context, SCE_GXM_CULL_NONE);
        } else {
            bool ccw = settings->windingOrder == WINDING_ORDER_COUNTER_CLOCKWISE;
            sceGxmSetCullMode(
                context,
                settings->cullMode == CULL_MODE_BACK ?
                (ccw ? SCE_GXM_CULL_CW : SCE_GXM_CULL_CCW) :
                (ccw ? SCE_GXM_CULL_CCW : SCE_GXM_CULL_CW)
            );
        }

        SceGxmPolygonMode polyMode = settings->fillMode == FILL_MODE_WIREFRAME
            ? SCE_GXM_POLYGON_MODE_LINE
            : SCE_GXM_POLYGON_MODE_TRIANGLE_FILL;
        
        sceGxmSetFrontPolygonMode(context, polyMode);
        sceGxmSetBackPolygonMode(context, polyMode);
    }


}