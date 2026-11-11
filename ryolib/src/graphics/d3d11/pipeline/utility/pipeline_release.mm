#include "../private.h"

void _pipeline_release(RYInternalPipeline* pipeline) {
    if (!pipeline) return;

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
