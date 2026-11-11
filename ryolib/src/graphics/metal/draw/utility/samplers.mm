#include "../private.h"

id<MTLSamplerState> _sampler_get_from_type(RYInternalRenderer& internalRenderer, RYSamplerType type) {
    switch (type) {
        case RYSamplerTypeNearest:
            return internalRenderer.nearestSampler;
        case RYSamplerTypeLinear:
            return internalRenderer.linearSampler;
        case RYSamplerTypeMinNearestMagLinear:
            return internalRenderer.minNearestMagLinearSampler;
        case RYSamplerTypeMinLinearMagNearest:
            return internalRenderer.minLinearMagNearestSampler;
        default:
            return nil;
    }
}
