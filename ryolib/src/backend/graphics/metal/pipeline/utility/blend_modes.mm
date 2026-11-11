#include "../private.h"

namespace ryo {
    void _pipeline_set_blend_mode(MTLRenderPipelineDescriptor* pipelineDesc, BlendMode blendMode) {
        switch (blendMode) {
            case BLEND_MODE_ALPHA: {
                pipelineDesc.colorAttachments[0].blendingEnabled = YES;
                pipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                pipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                pipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
                pipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                break;
            }
            case BLEND_MODE_ADDITIVE: {
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
}