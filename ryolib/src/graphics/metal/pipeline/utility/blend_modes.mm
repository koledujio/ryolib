#include "../private.h"

void _pipeline_set_blend_mode(MTLRenderPipelineDescriptor* pipelineDesc, RYBlendMode blendMode) {
    switch (blendMode) {
        case RYBlendModeAlpha: {
            pipelineDesc.colorAttachments[0].blendingEnabled = YES;
            pipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            pipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            break;
        }
        case RYBlendModeAdditive: {
            pipelineDesc.colorAttachments[0].blendingEnabled = YES;
            pipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
            pipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
            break;
        }
        default:
            break;
    }
}
