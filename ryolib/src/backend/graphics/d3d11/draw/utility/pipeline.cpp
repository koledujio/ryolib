#include "../private.h"

namespace ryo {
    bool _draw_bind_pipeline(_InternalRenderer& renderer, Pipeline pipeline, const DrawSettings* settings) {
        if (!pipeline_is_valid(pipeline)) return false;
        _InternalPipeline& ip = _pipeline_pool.items[pipeline.index()];

        renderer.context->IASetInputLayout(ip.inputLayout);
        renderer.context->VSSetShader(ip.vertexShader, nullptr, 0);
        renderer.context->PSSetShader(ip.pixelShader, nullptr, 0);
        renderer.context->OMSetBlendState(ip.blendState, nullptr, 0xffffffff);

        ID3D11RasterizerState* rasterizer = _draw_create_rasterizer(renderer.device, settings);

        if (rasterizer) {
            renderer.context->RSSetState(rasterizer);
            rasterizer->Release();
        }

        return true;
    }

    Pipeline _draw_select_pipeline(const DrawSettings* settings, Pipeline defaultPipeline) {
        if (settings && settings->pipelineOverride) return *settings->pipelineOverride;
        return defaultPipeline;
    }
}