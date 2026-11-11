#include "../private.h"

namespace ryo {
    bool _draw_bind_pipeline(InternalRenderer& renderer, const Pipeline* pipeline, const DrawSettings* settings) {
        if (!pipeline->valid()) return false;

        renderer.context->IASetInputLayout(pipeline->internal->inputLayout);
        renderer.context->VSSetShader(pipeline->internal->vertexShader, nullptr, 0);
        renderer.context->PSSetShader(pipeline->internal->pixelShader, nullptr, 0);
        renderer.context->OMSetBlendState(pipeline->internal->blendState, nullptr, 0xffffffff);

        ID3D11RasterizerState* rasterizer = _draw_create_rasterizer(renderer.device, settings);

        if (rasterizer) {
            renderer.context->RSSetState(rasterizer);
            rasterizer->Release();
        }

        return true;
    }

    const Pipeline* _draw_select_pipeline(const DrawSettings* settings, const Pipeline* defaultPipeline) {
        if (settings && settings->pipelineOverride) return settings->pipelineOverride;
        return defaultPipeline;
    }
}