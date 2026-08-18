#include "../private.h"

namespace ryo {
    id<MTLSamplerState> _sampler_get_from_type(_InternalRenderer& internalRenderer, SamplerType type) {
        switch (type) {
            case SAMPLER_TYPE_NEAREST:
                return internalRenderer.nearestSampler;
            case SAMPLER_TYPE_LINEAR:
                return internalRenderer.linearSampler;
            case SAMPLER_TYPE_MIN_NEAREST_MAG_LINEAR:
                return internalRenderer.minNearestMagLinearSampler;
            case SAMPLER_TYPE_MIN_LINEAR_MAG_NEAREST:
                return internalRenderer.minLinearMagNearestSampler;
            default:
                return nil;
        }
    }
}