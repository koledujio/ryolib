#include "../private.h"

namespace ryo {
    void _pipeline_release(InternalPipeline* pipeline) {
        if (pipeline->blendState) {
            pipeline->blendState->Release();
        }

        if (pipeline->inputLayout) {
            pipeline->inputLayout->Release();
        }

        if (pipeline->pixelShader) {
            pipeline->pixelShader->Release();
        }

        if (pipeline->vertexShader) {
            pipeline->vertexShader->Release();
        }

        delete pipeline;
    }
}