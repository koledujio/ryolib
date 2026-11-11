#include "../private.h"

namespace ryo {
    bool _pipeline_create_input_layout(const VertexLayoutType layout, const InternalRenderer& ir, InternalPipeline& pipeline, ID3DBlob* vertexBlob, HRESULT& hr) {
        switch (layout) {
            case VERTEX_LAYOUT_POSITION: {
                D3D11_INPUT_ELEMENT_DESC inputElements[] = {
                    {
                        "POSITION",
                        0,
                        DXGI_FORMAT_R32G32B32_FLOAT,
                        0,
                        static_cast<UINT>(offsetof(PositionVertex, position)),
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

            case VERTEX_LAYOUT_POSITION_UV: {
                D3D11_INPUT_ELEMENT_DESC inputElements[] = {
                    {
                        "POSITION",
                        0,
                        DXGI_FORMAT_R32G32B32_FLOAT,
                        0,
                        static_cast<UINT>(offsetof(PositionUVVertex, position)),
                        D3D11_INPUT_PER_VERTEX_DATA,
                        0
                    },
                    {
                        "TEXCOORD",
                        0,
                        DXGI_FORMAT_R32G32_FLOAT,
                        0,
                        static_cast<UINT>(offsetof(PositionUVVertex, uv)),
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
}