#include "../private.h"

namespace ryo {
    void _renderer_sampler_init(InternalRenderer& internalRenderer) {
        MTLSamplerDescriptor* nearestSamplerDesc = [[MTLSamplerDescriptor alloc] init];
        nearestSamplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
        nearestSamplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
        internalRenderer.nearestSampler = [internalRenderer.device newSamplerStateWithDescriptor:nearestSamplerDesc];
        [nearestSamplerDesc release];

        MTLSamplerDescriptor* linearSamplerDesc = [[MTLSamplerDescriptor alloc] init];
        linearSamplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        linearSamplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        internalRenderer.linearSampler = [internalRenderer.device newSamplerStateWithDescriptor:linearSamplerDesc];
        [linearSamplerDesc release];

        MTLSamplerDescriptor* nearLinSamplerDesc = [[MTLSamplerDescriptor alloc] init];
        nearLinSamplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
        nearLinSamplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        internalRenderer.minNearestMagLinearSampler = [internalRenderer.device newSamplerStateWithDescriptor:nearLinSamplerDesc];
        [nearLinSamplerDesc release];

        MTLSamplerDescriptor* linNearSamplerDesc = [[MTLSamplerDescriptor alloc] init];
        linNearSamplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        linNearSamplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
        internalRenderer.minLinearMagNearestSampler = [internalRenderer.device newSamplerStateWithDescriptor:linNearSamplerDesc];
        [linNearSamplerDesc release];
    }
}