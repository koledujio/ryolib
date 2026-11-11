#include "../private.h"

namespace ryo {
    void _draw_apply_settings(id<MTLRenderCommandEncoder> encoder, const DrawSettings* settings) {
        DrawSettings defaultSettings{};

        CullMode cull = settings ? settings->cullMode : defaultSettings.cullMode;
        WindingOrder winding = settings ? settings->windingOrder : defaultSettings.windingOrder;
        FillMode fill = settings ? settings->fillMode : defaultSettings.fillMode;

        MTLCullMode mtlCull;
        switch (cull) {
            case CULL_MODE_NONE:
                mtlCull = MTLCullModeNone;
                break;
            case CULL_MODE_FRONT:
                mtlCull = MTLCullModeFront;
                break;
            case CULL_MODE_BACK:
                mtlCull = MTLCullModeBack;
                break;
        }
        [encoder setCullMode:(MTLCullMode)mtlCull];

        MTLWinding mtlWinding = (winding == WINDING_ORDER_CLOCKWISE) ? MTLWindingClockwise : MTLWindingCounterClockwise;
        [encoder setFrontFacingWinding:mtlWinding];

        MTLTriangleFillMode mtlFill = (fill == FILL_MODE_WIREFRAME) ? MTLTriangleFillModeLines : MTLTriangleFillModeFill;
        [encoder setTriangleFillMode:mtlFill];
    }

    id<MTLRenderPipelineState> _draw_get_pso(const DrawSettings* settings, const Pipeline* defaultPipeline) {
        const Pipeline* pipeline = nullptr;

        if (settings != nullptr && settings->pipelineOverride != nullptr) {
            pipeline = settings->pipelineOverride;
        } else {
            pipeline = defaultPipeline;
        }
        
        if (pipeline == nullptr) return nil;
        if (!pipeline->valid()) return nil;

        return pipeline->internal->pso;
    }
}