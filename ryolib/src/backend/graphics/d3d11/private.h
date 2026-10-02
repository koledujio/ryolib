#pragma once

#include "../private.h"

#include "ryolib/templates.h"

#include "ryolib/graphics.h"

#include <chrono>
#include <d3d11.h>
#include <dxgi.h>

namespace ryo {
    struct InternalRenderer {
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        IDXGISwapChain* swapChain = nullptr;

        int width = 0;
        int height = 0;
        float contentScale = 1.0f;
        ID3D11RenderTargetView* colorView = nullptr;

        ID3D11Texture2D* depthTexture = nullptr;
        ID3D11DepthStencilView* depthView = nullptr;

        ID3D11DepthStencilState* depthStates[3] = { nullptr, nullptr, nullptr };
        ID3D11DepthStencilState* stencilWriteState = nullptr;
        ID3D11DepthStencilState* stencilTestStates[3] = { nullptr, nullptr, nullptr };

        Pipeline* defaultColorPipeline = nullptr;
        Pipeline* defaultTexturePipeline = nullptr;
        Pipeline* stencilPipeline = nullptr;

        Buffer* quadVertexBuffer = nullptr;
        Buffer* quadIndexBuffer = nullptr;
        Buffer* quadUVVertexBuffer = nullptr;
        Buffer* quadUVIndexBuffer = nullptr;

        ID3D11Buffer* vertexCB = nullptr;
        ID3D11Buffer* pixelCB = nullptr;

        ID3D11SamplerState* samplers[4] = { nullptr };
        ID3D11RasterizerState* rasterizers[3][2][2] = {{{ nullptr }}};

        bool inPass = false;
        bool inFrame = false;
        RenderTarget* currentTarget = nullptr;

        float deltaTime = 0.0f;
        std::chrono::steady_clock::time_point lastTime;
    };

    struct InternalRenderTarget {
        ID3D11RenderTargetView* colorView = nullptr;
        ID3D11DepthStencilView* depthView = nullptr;

        Texture* colorTexture = nullptr;
        Texture* depthTexture = nullptr;

        int width = 0;
        int height = 0;
    };

    struct InternalTexture {
        ID3D11Texture2D* texture = nullptr;
        ID3D11ShaderResourceView* srv = nullptr;
        int width = 0;
        int height = 0;
    };

    struct InternalBuffer {
        ID3D11Buffer* buffer = nullptr;
        BufferType type = BUFFER_TYPE_VERTEX;
        size_t size = 0;
    };

    struct InternalPipeline {
        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11InputLayout* inputLayout = nullptr;
        ID3D11BlendState* blendState = nullptr;

        VertexLayoutType vertexLayout = VERTEX_LAYOUT_POSITION_UV;
    };

    enum StencilShapeType {
        _STENCIL_SHAPE_RECTANGLE,
        _STENCIL_SHAPE_ROUNDED_RECTANGLE,
        _STENCIL_SHAPE_OVAL
    };

    struct StencilCommand {
        StencilShapeType shape;
        Matrix4 matrix;

        float radius;
        Camera camera;
        bool hasCamera;
    };

    struct StencilVertexUniforms {
        Matrix4 matrix;
    };

    struct StencilPixelUniforms {
        Vector2 scale;
        float radius;

        uint32_t shape;
    };

    struct InternalStencil {
        InternalRenderer* renderer = nullptr;
        InternalRenderTarget* renderTarget = nullptr;

        StencilCommand* commands = nullptr;
        size_t commandCount = 0;
        size_t commandCapacity = 0;
    };

    Matrix4 _draw_compute_camera_vp(const Camera& camera, float aspect);
    void _draw_get_target_size(const InternalRenderer& renderer, int& width, int& height);
}
