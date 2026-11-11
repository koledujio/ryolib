#include "../private.h"

namespace ryo {
    const Pipeline* _draw_active_pipeline_get(const Pipeline* defaultPipeline, const DrawSettings* settings) {
        if (settings && settings->pipelineOverride && settings->pipelineOverride->valid()) {
            return settings->pipelineOverride;
        }

        return defaultPipeline;
    }
}