#include "../private.h"

bool _pipeline_create_input_layout(const RYVertexLayoutType layout, const RYInternalRenderer& ir, RYInternalPipeline& pipeline, ID3DBlob* vertexBlob, HRESULT& hr) {
    switch (layout) {
        case RYVertexLayoutTypePosition: {
            D3D11_INPUT_ELEMENT_DESC inputElements[] = {
                {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    static_cast<UINT>(offsetof(RYPositionVertex, position)),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0
                }
            };

            hr = ir.device->CreateInputLayout(
                inputElements,
                1,
                vertexBlob->GetBufferPointer(),
                vertexBlob->GetBufferSize(),
                &pipeline.inputLayout
            );
            return true;
        }

        case RYVertexLayoutTypePositionUV: {
            D3D11_INPUT_ELEMENT_DESC inputElements[] = {
                {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    static_cast<UINT>(offsetof(RYPositionUVVertex, position)),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0
                },
                {
                    "TEXCOORD",
                    0,
                    DXGI_FORMAT_R32G32_FLOAT,
                    0,
                    static_cast<UINT>(offsetof(RYPositionUVVertex, uv)),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0
                }
            };

            hr = ir.device->CreateInputLayout(
                inputElements,
                2,
                vertexBlob->GetBufferPointer(),
                vertexBlob->GetBufferSize(),
                &pipeline.inputLayout
            );
            return true;
        }
    }

    return false;
}
